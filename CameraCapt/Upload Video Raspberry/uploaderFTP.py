from ftplib import FTP
import os

host = "Your Server"
uname = "Your Username FTP"
upass = "Your Password FTP"

ftp = FTP()
connect_server = ftp.connect(host=host, port=21, timeout=None)
print(connect_server)
login_status = ftp.login(user=uname, passwd=upass)
print(login_status)

ftp.cwd('BIOFARMA_SERVER/biofarma_wian/videos')
ct = 0

files = [f for f in os.listdir('.') if os.path.isfile(f)]
try:
    for f in files:
        fname, ext = os.path.splitext(f)
        if (ext == '.avi'):
            print('Uploading ' + f)
            ftp.storbinary('STOR ' + f, open(f, 'rb'))
            os.remove(f);
            ct += 1
    if ct > 0:
        print('Upload Success!\n' + str(ct) + ' files uploaded')
    else:
        print('No file uploaded')
except Exception as e:
    print(e)
