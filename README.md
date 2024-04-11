# ROBOTMAN
Code base for the very very famous ROBOT MAN!
For the time being, there is only one ROBOT MAN.
The code is callibrated for this specific unit.

## VSC extentions:
**C/C++** : ms-vscode.cpptools <br>
**Arduino** : vsciot-vscode.vscode-arduino <br>
**Processing Language** : Tobiah.language-pde <br>
**Command Runner** : edonet.vscode-command-runner <br>

## Setting up project using Serial monitor

Setting up Arduino for current ROBOTMAN - **Uno R3**
Create **arduino.json** file with following:
```json
{
    "board": "arduino:avr:uno",
    "sketch": "new/new.ino",
    "programmer": "atmel_ice",
    "port": "/dev/ttyACM0",
    "output": "robotman/build"
}
```

Setting up Arduino for future ROBOTMAN - **Uno R4**
Create **arduino.json** file with following:
```json
{
    "board": "arduino:avr:unowifi",
    "sketch": "new/new.ino",
    "port": "/dev/ttyACM0",
    "programmer": "avrispmkii",
    "output": "robotman/build"
}
```

### Uploading and running Arduino project
**Step 1** - Verify code
CTRL+ALT+R - Arduino Verify

**Step 2** - Upload code
CTRL+ALT+U - Arduino Upload

**Step 3** - Monitor
Switch to Serial monitor
Have the *Baud rate* be *9600*
Have the *Line ending* be *LF*
Click the *Start Monitoring*

## Setting up project with GUI using processing

#### Install processing

**Step 1:**
Download [Processing](https://processing.org/download)

**Step 2:**
Add processing to PATH
Instructions in *Processing Language* extention

**Step 3 for Ubuntu:**
Create a symlink for processing
```bash
sudo ln -s /opt/processing-4.3/ /bin/processing-java
```

**Step 3 for Windows:**
Go to *Processing Language Extention settings*.
Set *Processing Path* to e.g. 'C:\Program Files\processing-3.0.1\processing-java'

**Step 4:**
Create a **tasks.json** file with the following input:
```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Run Sketch",
      "type": "shell",
      "group": {
        "kind": "build",
        "isDefault": true
      },
      "command": "${config:processing.path}",
      "presentation": {
        "echo": true,
        "reveal": "always",
        "focus": false,
        "panel": "dedicated"
      },
      "args": [
        "--force",
        "--sketch=${workspaceRoot}/guiman",
        "--output=${workspaceRoot}/guiman/out",
        "--run"
      ],
      "windows": {
        "type": "process",
        "args": [
          "--force",
          {
            "value": "--sketch=${workspaceRoot}/guiman",
            "quoting": "strong"
          },
          {
            "value": "--output=${workspaceRoot}/guiman\\out",
            "quoting": "strong"
          },
          "--run"
        ]
      }
    }
  ]
}
```

### Running Robotman with GUI

CTRL+SHIFT+B - Run processing

### Upload/Flash sketch to UNO R4 Wi-Fi in VSCode

* If you don't have arduino-cli, download and install from this website https://arduino.github.io/arduino-cli/0.36/installation/.
* If you run Linux - start by making python script executable: ``chmod +x arduino_compile_upload_r4_wifi.py``

* Open Extension view in VSCode
* Search "Command Runner" and install it
* Open Command Palette (Ctrl+Shift+P) and select ``Preference: Open User Settings``
* Search for "Command Runner: Commands" and click ``Edit in settings.json``
* Add the following configuration to your settings.json file:
	```json
  "command-runner.commands": {
   		"Compile and Upload Sketch": "path/to/arduino_compile_upload_r4_wifi.py",
	}
  ```
* Now you can run the script by pressing Ctrl+Shift+R and choosing ``Compile and Upload Sketch``