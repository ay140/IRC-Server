# IRC Server Project

## Introduction

This project is an implementation of an IRC (Internet Relay Chat) server. IRC is a protocol for real-time Internet text messaging (chat) or synchronous conferencing. This project provides a basic IRC server that supports various IRC commands for client communication and channel management.

## Features

The IRC server supports the following commands:

### User Commands
- `PASS`: Set a password for the connection.
- `NICK`: Set or change the nickname of a user.
- `USER`: Set the username of a client.
- `OPER`: Grant operator status to a user.
- `MODE`: Change the user or channel mode.
- `PRIVMSG`: Send private messages between users.
- `NOTICE`: Send notices to users.
- `HELP`: Print help information.
- `INVITE`: Invite a user to a channel.
- `JOIN`: Join a channel.
- `TOPIC`: Change or view the topic of a channel.
- `KICK`: Eject a user from a channel.
- `PART`: Leave a channel.
- `SENDFILE`: Send a file to a user.
- `GETFILE`: Receive a file from a user.
- `BOT`: Interact with a bot.
- `CAP`: Handle capabilities.
- `WHOIS`: Get information about a user.
- `PING`: Ping the server to check connectivity.

## Command Details

### PASS
- Usage: `PASS <password>`
- Sets a connection password.

### NICK
- Usage: `NICK <nickname>`
- Sets or changes the nickname of a user.

### USER
- Usage: `USER <username> <hostname> <servername> <realname>`
- Sets the username and other details of a user.

### OPER
- Usage: `OPER <username> <password>`
- Grants operator status to a user.

### MODE
- Usage: `MODE <target> <mode> [<mode params>]`
- Changes the user or channel mode. Channel modes include:
  - `+i/-i`: Set/remove Invite-only channel.
  - `+t/-t`: Set/remove the restrictions of the TOPIC command to channel operators.
  - `+k <key>/-k`: Set/remove the channel key (password).
  - `+o <nick>/-o`: Give/take channel operator privilege.
  - `+l <limit>/-l`: Set/remove the user limit to the channel.

### PRIVMSG
- Usage: `PRIVMSG <target> <message>`
- Sends private messages to users.

### NOTICE
- Usage: `NOTICE <target> <message>`
- Sends notices to users.

### HELP
- Usage: `HELP`
- Prints help information about commands.

### INVITE
- Usage: `INVITE <nickname> <channel>`
- Invites a user to a channel.

### JOIN
- Usage: `JOIN <channel>`
- Joins a channel.

### TOPIC
- Usage: `TOPIC <channel> [<topic>]`
- Changes or views the topic of a channel.

### KICK
- Usage: `KICK <channel> <user> [<comment>]`
- Ejects a user from a channel.

### PART
- Usage: `PART <channel>`
- Leaves a channel.

### SENDFILE
- Usage: `SENDFILE <filename> <target>`
- Sends a file to a user.

### GETFILE
- Usage: `GETFILE <filename> <target>`
- Receives a file from a user.

### BOT
- Usage: `BOT <command>`
- Interacts with a bot.

### CAP
- Usage: `CAP <subcommand>`
- Handles capabilities.

### WHOIS
- Usage: `WHOIS <nickname>`
- Gets information about a user.

### PING
- Usage: `PING <server>`
- Pings the server to check connectivity.

## Installation and Usage

1. **Clone the repository:**
   ```sh
    git clone https://github.com/ay140/ft_IRC.git
    ```

2. **Compile the server:**
   ```sh
    make
    ```
3. **Run the server:**
   ```sh
    ./ircserv <port> <password>
    ```
4. **Connect to the server using an IRC client.**

### Installing Irssi

Irssi is a popular command-line IRC client that you can use to connect to this IRC server.

- On Ubuntu or Debian-based systems, use the following command:
  ```sh
  sudo apt install irssi
      ```
On macOS, especially for users at 42 schools using Homebrew, use:
  ```sh
brew install irssi
```


## Contact
For any inquiries or issues, please contact amarzouk@student.42abudhabi.ae.
