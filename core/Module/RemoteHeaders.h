#pragma once

namespace StarVFS {
namespace Modules {
namespace RemoteHeaders {

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

struct Settings {
	enum {
		BasePort = 55555,
		PortLimit = 10,
		MaxMessageSize = 128 * 1024,
	};
};

enum class Command : u32 {
//no payload
	NOP,
	Ping, Pong,
//metacommands
	Disconnect,
//rest
	GetFileTable,
	GetStringTable,
	GetFile,
};

struct MasterHeader {
	u32 RequestID;
	u32 PayLoadSize;
	Command Command;
	u32 ElementCount;

	u8 Payload[0];
};

struct GetFileRequest {
	enum class FindMode : u8 {
		Hash, ID, Path,
	};
	FindMode Mode;
	RWMode RWMode;
	OpenMode OpenMode;
	FileIDHash Hash;
	FileID ID;
	u8 Path[0];
};

struct GetFileResponse {
	u16 Result;
	u16 unused;
	u32 DataSize;
	u8 Data[0];
};

template<size_t LENGTH, class HEADER, class LOCKPOLICY = DefaultLockPolicy>
struct DynamicMessageBuffer {
	using Header = HEADER;
	using LockPolicy = LOCKPOLICY;
	enum {
		Size = LENGTH,
	};
	static_assert(std::is_pod<Header>::value, "DynamicMessageBuffer header mus be a POD type");

	DynamicMessageBuffer() : m_UsedSize(0), m_PullLocation(0) { Clear(); }


	void Fill(char value) {
		LockPolicy::Guard_t lock(m_Lock);
		memset(m_Buffer, value, Size);
	}
	void Clear() { m_PullLocation = m_UsedSize = HeaderSize(); }

	const char* GetBuffer() const { return m_Buffer; }
	size_t UsedSize() const { return m_UsedSize; }
	size_t HeaderSize() const { return sizeof(Header); }
	size_t PayLoadSize() const { return UsedSize() - HeaderSize(); }

	template<class T> bool Push(T & t) {
		Check<T>();
		LockPolicy::Guard_t lock(m_Lock);
		if (m_UsedSize + sizeof(T) > Size)
			return false;
		memcpy(m_Buffer + m_UsedSize, &t, sizeof(T));
		m_UsedSize += sizeof(T);
		return true;
	}
	template<class T> bool Push(T * t) {
		Check<T>();
		LockPolicy::Guard_t lock(m_Lock);
		if (m_UsedSize + sizeof(T) > Size)
			return false;
		memcpy(m_Buffer + m_UsedSize, t, sizeof(T));
		m_UsedSize += sizeof(T);
		return true;
	}
	bool PushString(const char *t) {
		size_t len = (t ? strlen(t) : 0) + 1;
		return PushString(t, len);
	}
	bool PushString(const char *t, size_t len) {
		LockPolicy::Guard_t lock(m_Lock);
		if (m_UsedSize + len > Size)
			return false;
		if (t)
			memcpy(m_Buffer + m_UsedSize, t, len);
		else
			m_Buffer[m_UsedSize] = 0;
		m_UsedSize += len;
		return true;
	}
	bool PushString(const std::string &t) {
		size_t len = t.length() + 1;
		LockPolicy::Guard_t lock(m_Lock);
		if (m_UsedSize + len > Size)
			return false;
		if (!t.empty())
			memcpy(m_Buffer + m_UsedSize, t.c_str(), len);
		else
			m_Buffer[m_UsedSize] = 0;
		m_UsedSize += len;
		return true;
	}
	template<class T> T* Alloc() {
		Check<T>();
		LockPolicy::Guard_t lock(m_Lock);
		if (m_UsedSize + sizeof(T) > Size)
			return nullptr;
		T *t = reinterpret_cast<T*>(m_Buffer + m_UsedSize);
		m_UsedSize += sizeof(T);
		return t;
	}
	template<class T> T* AllocAndZero() {
		auto *t = Alloc<T>();
		if (!t) return t;
		memset(t, 0, sizeof(T));
		return t;
	}

	Header* GetHeader() {
		return reinterpret_cast<Header*>(m_Buffer);
	}

	template<class T> void Pull() {
		Check<T>();
		LockPolicy::Guard_t lock(m_Lock);
		m_PullLocation += sizeof(T);
	}
	const char* PullString() {
		const char *c = m_Buffer + m_PullLocation;
		LockPolicy::Guard_t lock(m_Lock);
		for (; m_Buffer[m_PullLocation]; ++m_PullLocation);
		if (!m_Buffer[m_PullLocation])
			++m_PullLocation;
		return c;
	}
	const char* PullBytes(size_t size, char* out = nullptr) {
		const char *c = m_Buffer + m_PullLocation;
		LockPolicy::Guard_t lock(m_Lock);
		if (out)
			memcpy(out, c, size);
		m_PullLocation += size;
		return c;
	}
	template<class T> T* Get() {
		Check<T>();
		return reinterpret_cast<T*>(m_Buffer + m_PullLocation);
	}
	template<class T> T* GetAndPull() {
		Check<T>();
		LockPolicy::Guard_t lock(m_Lock);
		T* t = Get<T>();
		Pull<T>();
		return t;
	}

private:
	LockPolicy m_Lock;
	size_t m_UsedSize;
	size_t m_PullLocation;
	char m_Buffer[Size];

	template<class T>
	void Check() {
		static_assert(std::is_pod<T>::value, "DynamicMessageBuffer accepts only POD types");
	}
};

struct MemoryNoLockPolicy {
	struct Guard_t { Guard_t(MemoryNoLockPolicy&) {} };
	void lock() {}
	void unlock() {}
};

using MessageBuffer = DynamicMessageBuffer<Settings::MaxMessageSize, MasterHeader, MemoryNoLockPolicy>;

template <class IOSERVICE, class SOCKET>
struct BaseConnection {
	IOSERVICE m_io_service;
	SOCKET m_Socket;

	BaseConnection(): m_io_service(), m_Socket(m_io_service) { }

	virtual bool ProcessCommand(MessageBuffer &message) = 0;
	virtual bool CanRun() const = 0;

	bool SendAndWait(MessageBuffer &message) {
		Command UntilCommmand = message.GetHeader()->Command;
		if (!WriteMessage(message))
			return false;
		return DispatchPendingCommand(message, UntilCommmand);
	}

	bool WaitForResponse(MessageBuffer &message) {
		Command UntilCommmand = message.GetHeader()->Command;
		if (!WriteMessage(message))
			return false;
		while (true) {
			if (!ReadMessage(message))
				return false;
			if (message.GetHeader()->Command == UntilCommmand)
				return true;
			if (!ProcessCommand(message))
				return false;
		}
		return false;
	}

	bool DispatchPendingCommand(MessageBuffer &message, Command UntilCommmand = Command::NOP) {
		bool done = UntilCommmand == Command::NOP;
		while (true) {
			if (done && m_Socket.available() <= 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				if (m_Socket.available() <= 0)
					return true;
			}
			if (!ReadMessage(message))
				return false;
			if (message.GetHeader()->Command == UntilCommmand)
				done = true;
			if(!ProcessCommand(message))
				return false;
		}
		return true;
	}

	bool ReadCommands(MessageBuffer &message) {
		while (true) {
			if (!ReadMessage(message) || !ProcessCommand(message))
				return false;
		}
		return true;
	}

	bool WriteMessage(MessageBuffer &message) {
		auto hdr = message.GetHeader();
		hdr->PayLoadSize = message.PayLoadSize();

		size_t remain = message.UsedSize();
		auto data = message.GetBuffer();
		while (CanRun() && remain > 0) {
			boost::system::error_code error;
			size_t written = m_Socket.write_some(boost::asio::buffer(data, remain), error);
			data += written;
			remain -= written;
			if (error) {
				STARVFSDebugLog(" FAILED: Send command:%d payload:%d", hdr->Command, hdr->PayLoadSize);
				return false;
			}
		}
		STARVFSDebugInfoLog("Send command:%d payload:%d", hdr->Command, hdr->PayLoadSize);
		return true;
	}

	bool ReadMessage(MessageBuffer &message) {
		auto hdr = message.GetHeader();
		hdr->Command = Command::NOP;
		message.Clear();

		auto ReadRemain = [&message, this](char *start, size_t remain) ->bool {
			while (CanRun() && remain > 0) {
				boost::system::error_code error;
				size_t length = m_Socket.read_some(boost::asio::buffer(start, remain), error);
				if (error == boost::asio::error::eof)
					return false;
				else if (error) {
					m_Socket.close();
					return false;
					//	throw boost::system::system_error(error); // Some other error.
				}
				remain -= length;
				start += length;
			}
			return true;
		};

		if (!ReadRemain((char*)message.GetBuffer(), message.HeaderSize()))
			return false;

		if (!ReadRemain((char*)message.GetBuffer() + message.HeaderSize(), hdr->PayLoadSize))
			return false;

		STARVFSDebugInfoLog("Recived command:%d payload:%d", hdr->Command, hdr->PayLoadSize);
		return true;
	}
};

} //namespace RemoteHeaders
} //namespace Modules 
} //namespace StarVFS 
