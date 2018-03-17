# starVFS
An extensible virtual file system 

## Features
* File lookup is done by 32bit hash of file path
* Filea read/write is done by handle interface
* Has modular design 
* Several modules can be mounted at any point in path
* Have dedicated file container format RDC for fastest mount and data load speed
* Command line interface for importing/exporting RDC containers
* Has dedicated client/server modules to mount or expose contetn via tcp connection
* Two Total Commander plugins: RDC container reader and tcp client

## RDC container
* Uses section layout to store data
* Supports zLib compression (can extended to support more types)
* Encryption can be added while maintaining backward capability
* Encryption or compression can be per file or per data section in file
* Can hold special hash sections for faster load into starVFS
