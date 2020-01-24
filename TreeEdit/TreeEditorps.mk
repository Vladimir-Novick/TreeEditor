
TreeEditorps.dll: dlldata.obj TreeEditor_p.obj TreeEditor_i.obj
	link /dll /out:TreeEditorps.dll /def:TreeEditorps.def /entry:DllMain dlldata.obj TreeEditor_p.obj TreeEditor_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del TreeEditorps.dll
	@del TreeEditorps.lib
	@del TreeEditorps.exp
	@del dlldata.obj
	@del TreeEditor_p.obj
	@del TreeEditor_i.obj
