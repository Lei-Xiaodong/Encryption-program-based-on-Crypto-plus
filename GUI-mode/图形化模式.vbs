sub f(oExec)
Do While Not oExec.StdOut.AtEndOfStream
    cmdtext = oExec.StdOut.ReadAll()
Loop
msgbox cmdtext
end sub

Dim a,msg,p1,p2,cmdtext
msg = "����ѡ��:(����0�˳�)" &vbcrlf& "    1.des����;            2.des����;" &vbcrlf&vbcrlf& "    3.����ժҪ;           4.�Ƚ�ժҪ;" &vbcrlf&vbcrlf& "    5.����ǩ��;           6.�Ƚ�ǩ��;" &vbcrlf&vbcrlf& "    7.�����ŷ�;           8.���ŷ�"

do

a=Inputbox(msg,"���˵�") 
if a=0 then exit do end if
if 0<a<9 then

Set r = CreateObject("WScript.Shell")
p1=inputbox("�����ļ�·��:","����")
p2=inputbox("����ļ�·��:","���")

select case a
case 1  Set oExec = r.Exec("%COMSPEC% /C ��̨����.exe des1 "&p1&" "&p2)
	call f(oExec)
	r.run p2,1
	
case 2  Set oExec = r.Exec("%COMSPEC% /C ��̨����.exe des0 "&p1&" "&p2)
	call f(oExec)
	r.run p2,1
	
case 3  Set oExec = r.Exec("%COMSPEC% /C ��̨����.exe md5a "&p1&" "&p2)
	call f(oExec)
	r.run p2,1
	
case 4  Set oExec = r.Exec("%COMSPEC% /C ��̨����.exe md5b "&p1&" "&p2)
	call f(oExec)
	
case 5  Set oExec = r.Exec("%COMSPEC% /C ��̨����.exe sign1 "&p1&" "&p2)
	call f(oExec)
	r.run p2,1

case 6  Set oExec = r.Exec("%COMSPEC% /C ��̨����.exe sign0 "&p1&" "&p2)
	call f(oExec)
	r.run p2,1

case 7  Set oExec = r.Exec("%COMSPEC% /C ��̨����.exe alluse1 "&p1&" "&p2)
	call f(oExec)
	r.run p1,1

case 8  Set oExec = r.Exec("%COMSPEC% /C ��̨����.exe alluse0 "&p1&" "&p2)
	call f(oExec)
	r.run p1,1
end select

end if
loop while a



