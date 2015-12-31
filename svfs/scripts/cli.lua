
cli = inst.cli

Help.Register { Command="exit", Brief="exit from cli", Usage="exit(code)"}
function exit(code)
    code = code or 0
    cli:Exit(code)
end
