#!/usr/bin/python3

from tkinter import *
import os
from subprocess import run, STDOUT, PIPE

BUTTON_H = 3
BUTTON_W = 50
BUTTON_F = 20

text = None

def runcmd(cmd, cwd):
	output = run(cmd.split(), stdout=PIPE, stderr=STDOUT, text=True, cwd=cwd)
	outstr = output.stdout
	if output.stderr:
		outstr += output.stderr
	text.insert(1.0, outstr + '\n')
	if output.returncode == 0:
		text['fg'] = '#44CC44'
	elif output.returncode < 0:
		text['fg'] = '#CC4444'
	else:
		text['fg'] = '#AAAACC'
	return output.returncode

def gitpull():
	cmd = '/usr/bin/git pull'
	runcmd(cmd, os.getcwd())

def rebuild_esecamd():
	cmd = '/bin/bash build.sh'
	runcmd(cmd, os.getcwd() + '/../eseircam/daemon/TargetBuild/')

def rebuild_gui():
	cmd = '/usr/bin/make clean'
	runcmd(cmd, os.getcwd() + '/../build-eyemetergui-Desktop-Release/')
	cmd = '/usr/lib/qt5/bin/qmake ../eyemetergui/'
	if runcmd(cmd, os.getcwd() + '/../build-eyemetergui-Desktop-Release/') == 0:
		cmd = '/usr/bin/make -j4'
		runcmd(cmd, os.getcwd() + '/../build-eyemetergui-Desktop-Release/')

def flash_mcu():
	cmd = '/bin/bash update.sh'
	runcmd(cmd, os.getcwd() + '/../led/esp/')

def rebuild_90():
	cmd = '/bin/bash build_90hz.sh'
	runcmd(cmd, os.getcwd() + '/../eseircam/daemon/TargetBuild/')

if __name__ == "__main__":
	root = Tk()
	root.title('Updater')
	frame = Frame(root, width=400, height=350)
	frame.grid(row=0, column=0)
	text = Text(frame, bg='#444444', width=100)
	text.grid(row=0,column=0)
	but_pull = Button(height=BUTTON_H, width=BUTTON_W, text='git pull', font=BUTTON_F, command=gitpull)
	but_pull.grid(row=1,column=0)
	but_reese = Button(height=BUTTON_H, width=BUTTON_W, text='rebuild daemon', font=BUTTON_F, command=rebuild_esecamd)
	but_reese.grid(row=2,column=0)
	but_regui = Button(height=BUTTON_H, width=BUTTON_W, text='rebuild gui', font=BUTTON_F, command=rebuild_gui)
	but_regui.grid(row=3,column=0)
	but_flash = Button(height=BUTTON_H, width=BUTTON_W, text='flash mcu', font=BUTTON_F, command=flash_mcu)
	but_flash.grid(row=4,column=0)
	but_rebuild_90 = Button(height=BUTTON_H, width=BUTTON_W, text='rebuild_90', font=BUTTON_F, command=rebuild_90)
	but_rebuild_90.grid(row=5,column=0)

	root.mainloop()
