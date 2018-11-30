[screen-tray-icon]: res/screen-tray-icon.png "Tray icon"
[screen-tray-device]: res/screen-tray-device.png "Device selection dialog"
[screen-tray-rules]: res/screen-tray-rules.png "Filter rules dialog"
[screen-gwconfig-index]: res/screen-gwconfig-index.png "Gateway configuration"

# SElink user's manual

## Introduction
SElink is a software which allows to establish secure connections using the SEcube open-source security platform. There is no need to adopt specific SEcube-enabled software, as SElink can encrypt connections from any existing application transparently.

### Naming conventions

* **Client**: the host which initiates a connection
* **Server**: the host which accepts the connection

### Client and Server

The software is made of two main components:

* Client software
* Server software

In order to establish a secure communication, a properly configured host running the client software must initiate a connection towards a properly configured host running the server software.
In most cases the user will only need to install and configure the client software.

### Requirements and Applicability

Client requirements:

* 64-Bit Windows 10
* SEcube device\*

Server requirements:

* Any of:
    * Windows\*\* 7 or newer
    * Linux
* SEcube device\*

\* SElink can be used without a SEcube device, for testing purposes or custom configurations.
\*\* Daemon mode not yet supported on Windows.

## Client Software

### Installing

There are three components to be installed for the software to function properly:

1. SElink driver
1. SElink service
1. SElink GUI

The provided setup executable installs all the components at once. Refer to the installer source for manual installation instructions.

Since the driver is not signed, Windows needs to run in Test Mode to install the driver.

1. Open an administrator command prompt and execute the following command:
    `bcdedit /set TESTSIGNING ON`
	*Warning: this command disables driver signature check enforcement on Windows.*
1. Reboot

Finally, run `selink-setup.exe`.

### Configuring

The SElink client software can be easily configured through the GUI application.

#### Device

Access the device configuration window by left-clicking the tray icon and choosing `Device`

![SElink GUI tray icon][screen-tray-icon]
![Device selection dialog][screen-tray-device]

##### Service and device status

The current status of the service and the device is shown on the top.

##### Connect a SEcube device

1. Select a suitable device from the drop-down menu. If a device does not show up, press the refresh button on the right.
1. Press the `Connect` button
1. Insert your user pin and click the `Login` button
1. The device configuration window should disappear, and a notification should appear shortly after.

##### Disconnect a SEcube device

Press the `Disconnect` button


#### Filter rules

Access the filter rules configuration window by left-clicking the tray icon and choosing `Filter rules`.

![Filter rules dialog][screen-tray-rules]

##### About filter rules

Filter rules are used to manage outgoing connections, to decide which will be encrypted and with which key. A filter rule is made of:

* A condition to select connections based on some parameters:
    * **Executable path**: full path to the source application's executable file
    * **Destination IP**: destination IP address
    * **First port**, **Last port**: destination port range
* An action to be taken with the matching connections:
    * **Action**: one of *Allow*, *Block* or *Encrypt*
    * **Key**: the key id to be used when encrypting

In order for a condition to match a connection, all the parameters must match. An **empty parameter** stands for **any value**.

Within the GUI you may define a list of filter rules, to select different actions for different connections. For example, you may assign a different encryption key to each application you are going to use.
Note that the **order** in which rules appear **is important**: the first matching rule is chosen, regardless of the following rules.
If a connection does not match any rule, it is allowed (not encrypted) by default.

##### Add a rule

Create a new rule by doing any of the following:

* Right-click on the grid and select `Insert after` or `Insert before`
* Fill the last row of the grid

The rules' fields must comply with the following constraints:

* **Description** must be shorter than **64 characters**
* **Destination IP** must be a valid **ipv4** or **ipv6** address
* **First port** and **last port** must describe a **valid port range**
  * `first port`, `last port` must be both natural numbers lesser than 65536, or both empty
  * if not empty, `first port` must be lesser than or equal to `last port`
* The **key** must be
  * An integer number if the action is *Encrypt*
  * Empty if the action is *Allow* or *Block*

> Tip: you can override the default action by adding a rule with empty condition fields at the end of the list.

##### Move a rule

Drag and drop a row over the desired position.

##### Delete a rule

Right-click on a row and select `Delete`.

## Server software

### Installation

1. Install g++ and the boost development libraries for your system

    e.g `# pacman -S base-devel boost` on Arch Linux

1. `cd` into the SElink source code directory and build the SElink gateway

    `$ make`

1. Install

    * Default installation

        1. `# make install`
    	    *The software and configuration files will be installed in `/opt/selink/`*
            *It will be configured to run without a SEcube device, using the keys from `/opt/selink/keys.json`*
            *The systemd unit will be installed in `/usr/lib/systemd/system/selinkgw.service`*

    * Custom installation

        * Copy `bin/selinkgw` and any needed configuration file to a directory of your choice
        * Customize the systemd unit in example/selinkgw.service (read section <a href="#selink-gateway-options">SElink gateway options</a>) and copy it to the appropriate location for your system


### Running

Start the daemon: `# systemctl start selinkgw`
Stop the daemon: `# systemctl stop selinkgw`

### Configuring

The configuration is made of a list of port mappings. Each entry contains:
* A description text
* The port on which encrypted connections will be accepted
* The host and port to which connections will be redirected, unencrypted
* The key id to use for encryption

You may configure the SElink gateway in two ways
* Using the web UI
* Using the configuration file

#### Using the web UI

1. Install python3 and python3 modules `bottle`, `jsonschema` on your system
    e.g. `# pacman -S python python-bottle python-jsonschema` on Arch Linux
2. `cd` into the `gwconfig` directory and run the Web UI as root (assumes a default installation)
    `# python3 gwconfig.py --use-token`
3. Open the link on the terminal output with a web browser

![][screen-gwconfig-index]

##### Add a mapping

Press `Add` and insert the rule

The mappings' fields must comply with the following constraints:

* `Listen port` and `Redirect port` must be natural numbers lesser than 65536
* `Redirect host` must be a valid ipv4 or ipv6 address

##### Delete a mapping

Press the trash icon in the last column of the row to delete

##### Save and apply

Press the `Apply` button. The configuration will be saved and applied immediately.

#### Using the configuration file

1. Edit the configuration file with a text editor. See `example/selinkgw.json` for an example.
2. Signal the daemon: `# systemctl reload selinkgw`

## Advanced

### SElink driver parameters

The driver can be configured to only filter connections with destination ports within a port range, and allow anything else, regardless of whether the service is running.

The driver configuration is stored in the following registry key:
    `HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Service\selinkflt\Parameters`

| Name <div class="fwc"></div> | Type | Description |
|--|--|--|
| `PortFirst` | `REG_DWORD` | First port of filtered port range |
| `PortLast` | `REG_DWORD` | Last port of filtered port range |
| `ServicePort` | `REG_DWORD` | Port on which the service is listening for redirected connections |

### Command line
#### Path conventions
Any path starting with `:` is relative to the executable's path. For example, if the executable is in `C:\SElink\selinksvc.exe`, then `:selinksvc.json` points to `C:\SElink\selinksvc.json`.

Any relative path is relative to the current working directory, which depends on how the process was created.

#### INI files

Command line options for the SElink service and SElink gateway may be specified either by passing them as parameters on the command line, or setting them in special configuration files.

For SElink service, create a file named `selinksvc.ini` in the same directory as the executable.

For SElink gateway, create a file named `selinkgw.ini` in the same directory as the executable.

Example ini file:
```
provider=soft
keys=:keys.json
```

Only long options are allowed in the configuration file


#### SElink service options

| Option <div class="fwc"></div> | Value <div class="fwc"></div> | Description |
|--|--|--|
| `--help`, `-h`  | (None) | Show a help message |
| `--log`, `-l` | log file path | Set the location of the log file. Defaults to `:selinksvc.log` when running as service, or none when running in foreground. |
| `--config`, `-c` | configuration file path | Set the location of the filter rules configuration file. Defaults to `:selinksvc.json`. |
| `--provider`, `-p` | provider type | Set the provider type. Can be one of `soft`, `secube`. |
| `--keys`, `-k` | path to key collection file | Set the keys. Only required if the provider is of type `soft`. |
| `--port`, `-w` | port | Set the driver connection redirection port. See <a href="#selink-driver-parameters">SElink driver parameters</a> |
| `--foreground`, `-f` | (None) | Run in foreground instead of running as a service. |

#### SElink gateway options

| Option <div class="fwc"></div> | Value <div class="fwc"></div> | Description |
|--|--|--|
| `--help`, `-h` | (None) | Show a help message |
| `--log`, `-l` | log file path | Set the location of the log file. Defaults to `/var/log/selinkgw.log` when running as service, or none when running in foreground. |
| `--config`, `-c` | configuration file path | Set the location of the filter rules configuration file. Defaults to `:selinkgw.json`. |
| `--provider`, `-p` | provider type | Set the provider type. Can be one of `soft`, `secube`. |
| `--keys`, `-k` | path to key collection file | Set the keys. Only required if the provider is of type `soft`. |
| `--serial-number`, `-s` | path to key collection file | Set the keys. Only required if the provider is of type `soft`. |
| `--pin`, `-z` | path to pin file | The user pin to log into the SEcube device will be read from the specified file. Only required if the provider is of type `secube`. |
| `--foreground`, `-f` | (None) | Run in foreground instead of running as daemon. |

#### SElink gateway web UI options
| Option <div class="fwc"></div> | Value <div class="fwc"></div> | Description |
|--|--|--|
| `--help`, `-h` | (None) | Show a help message |
| `--host` | host | Host on which the web UI server will listen |
| `--port` | port | Port on which the web UI server will listen |
| `--config`, `-c` | configuration file path | Path to gateway configuration file |
| `--pidfile` | pidfile path | Path to gateway pidfile |
| `--use-token` | (None) | Generate a random token to restrict access to the web UI |
| `--debug` | (None) | Enable debug mode |

### Configuration files

#### Filter rules

Refer to the schema <a href="res/rules-schema.json">rules-schema.json</a>.

#### Gateway

Refer to the schema <a href="res/config-schema.json">config-schema.json</a>.

#### Key collection

Refer to the schema <a href="res/keys-schema.json">keys-schema.json</a>.

### Manual installation

Refer to the Inno Setup installer script found in `installer/selink.iss`

<style>
div.fwc { width:200px; }
</style>
