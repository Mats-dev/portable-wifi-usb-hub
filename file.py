from flask import Flask, request, jsonify, render_template_string
import os
import subprocess

app = Flask(__name__)

MOUNT_POINT = "/media/usb"

def scan_drives():
    result = subprocess.run(['lsblk', '-o', 'NAME,SIZE,MOUNTPOINT'], stdout=subprocess.PIPE)
    return result.stdout.decode('utf-8')

def mount_drive(drive):
    mount_point = f"{MOUNT_POINT}/{os.path.basename(drive)}"
    if not os.path.ismount(mount_point):
        os.system(f"sudo mkdir -p {mount_point}")
        os.system(f"sudo mount {drive} {mount_point}")
    return f"Drive {drive} mounted successfully at {mount_point}"

def unmount_drive(drive):
    mount_point = f"{MOUNT_POINT}/{os.path.basename(drive)}"
    if os.path.ismount(mount_point):
        os.system(f"sudo umount {mount_point}")
    return f"Drive {drive} unmounted successfully"

def eject_drive(drive):
    mount_point = f"{MOUNT_POINT}/{os.path.basename(drive)}"
    if os.path.ismount(mount_point):
        os.system(f"sudo umount {mount_point}")
        os.system(f"sudo eject {drive}")
    return f"Drive {drive} ejected successfully"

def get_drive_usage():
    result = subprocess.run(['df', '-h', MOUNT_POINT], stdout=subprocess.PIPE)
    return result.stdout.decode('utf-8')

@app.route('/')
def index():
    return render_template_string(open('/var/www/html/index.html').read())

@app.route('/scan', methods=['GET'])
def scan():
    return scan_drives()

@app.route('/mount', methods=['GET'])
def mount():
    drive = request.args.get('drive')
    return mount_drive(drive)

@app.route('/unmount', methods=['GET'])
def unmount():
    drive = request.args.get('drive')
    return unmount_drive(drive)

@app.route('/eject', methods=['GET'])
def eject():
    drive = request.args.get('drive')
    return eject_drive(drive)

@app.route('/usage', methods=['GET'])
def usage():
    return get_drive_usage()

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)