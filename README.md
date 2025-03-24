<div align="center">


</div>

<div align="center">
  <h1>What is this all about?</h1>
</div>

<br>The **ft_irc** project is an implementation of an IRC (Internet Relay Chat) server designed to simulate the basic functionality of an IRC server used for real-time communication between users in channels. 
Built with C++98, this project aims to create a simple yet functional IRC server that supports essential commands such as `JOIN`, `PART`, `NICK`, `USER`, and more.

The project is split into two versions:

1. **Base Version**: The basic IRC server, where you can connect clients, join channels, send messages, and manage nicknames.
2. **Bonus Version**: An extended version that includes a bot, adding additional functionality and interactivity to the server, allowing users to engage with automated responses or other features.

The goal of this project is not only to provide a working chat system but also to enhance the understanding of network programming, client-server interactions, and socket management in C++. 
It allows users to create channels, join them, send private messages, and manage their own nicknames, all while interacting with other clients on the server.

By building this project, you will learn about handling multiple client connections, managing real-time communication in a server-client model, and dealing with the intricacies of networking protocols.

Whether you're new to networking, C++, or just want to build something practical, this project gives you a hands-on experience with the IRC protocol and lays the foundation for more complex server-client applications.

<br>
<br>

>[!NOTE]
>This repository is open and you are encouraged to contribute. <br>
>If you have something to add or a tip to give, feel free to do so.



<div align="center">
  <h1>How can I test it?</h1>
</div>
<div align="center">
<p>I'm glad you asked! The process is very simple.</p>
</div>
<br>
<h4>1º Step - Download the files:</h4>

```bash
git clone https://github.com/brennm2/IRCServer.git | cd IRCServer
```

<h4>2º Step - Compiling the program</h4>

```bash
make
```
I recommend using bonus version, so:
```bash
make bonus
```
<br>
<h4>3º Step - Running the server!</h4>

Now you need to put in the following inputs:<br>
- ft_irc version (irc_serv or irc_servbonus)<br>
- Port (ex 6667)<br>
- Password (ex 1234)<br>
```bash
./ircserv_bonus 6667 1234
```

<h4>4º Step - Join the Server!</h4>
<p>By Netcat:</p>

```bash
nc -C localhost 6667
pass 1234
user bde-souz
nick brendon 0 * :Brendon Vianna
```
<br>or Hexchat:</br>
<div align="center">
  <img src="https://i.imgur.com/TtRzJjQ.png"style="width:35%">
</div>



<div align="center">
  <h1>Considerations and Explanations</h1>
</div>
<div align="center">
<h3>--------- Commands ---------</h3>


| Command        | Description                                                                 |
|:----------------|-----------------------------------------------------------------------------|
| `/join <#channel1,#chennel2...> <password> or /join 0`    | Joins the specified channel or leaves all channels             |
| `/part <#channel1,#channel2...> [reason]`    | Leaves the specified channel.                                               |
| `/nick <nickname>`    | Changes your nickname to the specified one.                                  |
| `/privmsg <user1,user2...> [message]` | Sends a private message to the specified user.                              |
| `/topic <#channel> or <#channel> [topic]`    | Displays or sets the topic for a channel.                                    |
| `/quit [reason]`     | Quits the IRC server with an optional message.                               |
| `/invite <nickname> <#channel>` | Invite a user to a channel.                                               |
| `/kick <#channel> <nickname1,nickname2...> [reason]` | Kick a user from the channel.                                               |
| `/invite <nickname> <#channel>` | Invite a user to a channel.                                               |
| `/mode <#channel> <mode> <target or option>` | Set a mode to a channel.                                               |
| `/motd` | Tells the message of the day.                                               |
| `/BOT [HELP]` | Shows commands for the bot (**ONLY IN BONUS**)                                               |
</div>
<br>
<div align="center">
<h3>--------- The Modes ---------</h3>


| Modes        | Description                                                                 |
|:----------------|-----------------------------------------------------------------------------|
| `/mode <#channel> +o or -o <target>`    | Gives or remove operator to a user.                                                 |
| `/mode <#channel> +i or -i`    | Change or remove the channel to Invite Only.                                                 |
| `/mode <#channel> +k or -k <option>`    | Put or remove a password to a channel.                                                 |
| `/mode <#channel> +t or -t`    | Lock or unlock the topic of channel.                                                 |
| `/mode <#channel> +l or -l <option>`    | Set or unset a limit of user to a channel.                                                 |
</div>
<br>

<div align="center">
<h3>--------- Important Notes! ---------</h3>
</div>

<h3>
  
- IRC Protocol **[is case-insensitive](https://www.seobility.net/en/wiki/images/c/c5/Case-sensitivity.png)**
</h3>

>[!WARNING]
>Be careful with the letters!.

Yes, this is **VERY** important, if you enter a nick like `bde-souz` and then other an user joins with `BDE-SOUZ`, it can steal the identity of the first user,
and that is a **VERY** big problem! Same with the channel, that can leave to pretty bad problems!

<div align="center">
Here are some examples:

<br>
<br>

| Example        | Description                                                                 |
|:----------------:|:-------------------------------------------------------------------------:|
| `bde-souz and BDE-SOUZ`    | It should give a 433 error (Nickname is already in use).        |
| `#channel and #CHANNEL`    | It should JOIN in the first version that was created! Ex: #channel.|
| `join and JOIN`    | Commands should be interpreted as the same. Ex: JOIN and join |
</div>
<br>
<h3>
<br>
<br>
- SIGPIPES
</h3>

>[!IMPORTANT]
>No one likes broken pipes!.

This can happen if you send a message to a `closed Fd`, you can easly do this with a scipt in shell that closes multiple clients at once (ex Ctrl + C)

<div align="center">
Here are some quick solutions:

| Solutions        | Description                                                                 |
|:----------------:|:-------------------------------------------------------------------------:|
| `signal(SIGPIPE, SIG_IGN)`    | Use signal to ignore the SIGPIPE throw.        |
| `send(clientFd, message, message.size, MSG_NOSIGNAL)`    | Enable the MSG_NOSIGNAL flag to disable throws of send().|
| `do not send`    | Well... find a way to not send the message to that FD ¯\\\_(ツ)\_/¯ |
</div>
<h3>
<br>
<br>
- Arguments and Texts
</h3>

>[!TIP]
>Dots, dots and dots....

Be careful, text are represented with `:`, so what happens if you don't put it? It should be considered an argument!
So... with should only consider the first word

<div align="center">
Here are some examples:
  
| Example        | Output                                                                |
|:----------------:|:-------------------------------------------------------------------------:|
| `privmsg #channel :Hello World :D`    | bde-souz: Hello World :D       |
| `privmsg #channel Hello World :D`    | bde-souz: Hello     |
| `kick #channel bde-souz :You behaved bad!`    | bde-souz has been kicked from #teste (You behaved bad!) |
| `kick #channel bde-souz You behaved bad!`    | bde-souz has been kicked from #teste (You) |
| `etc`    | You got the idea :) |
</div>
<br>
<br>
<h3>
- HexChat and Netcat
</h3>

>[!TIP]
>Hexchat autocompletes things and Netcat accepts everything!.

You need to be careful with Hexchat and Netcat!<br>

In the case of the Hexchat, it usually autocompletes the commands, for example, if you send `/privmsg Hello World` in the Hexchat, it will autocomplete the command,
as in netcat, it will accept any reply, even if the syntax is wrong, use the hexchat to check it!

<p>You can see that you need to use both of them to verify if everything is good

| Input        | Model  | Output in the Server  |
|:-------------------:|:-------:|:---------------------------:|
| `/mode +o #bde-souz`| Hexchat | _mode #general +o #bde-souz_ |
| `hello world` (in channel)| Hexchat | _PRIVMSG #teste :hello world_ |
| `/part` (in channel)| Hexchat | _PART #teste :Leaving_ |
| `/nick brendon` | Netcat | :brendon!~Brendon@localhost NICK :brendon  **(WRONG SYNTAX)**|
| `/nick bde-souz` | Netcat | : 433 bde-souz :Nickname is already in use  **(WRONG SYNTAX)**|



<div align="center">
<h3>--------- 42 Rules ---------</h3>
</div>
<br>
<p>In every project we must follow certain rules, here are the rules for this project:</p>

```bash
- The project must not have memory leaks
- The project must use the flags -Wall -Wextra -Werror and use C++98
```

<br>
<br>
<div align="center">

<h3>--------- Credits ---------</h3>
</div>

* My awesome duo in this project: [Diogo](https://github.com/diogocorreia71)
* [IRC Protocol]
    - [IRC Docs](https://modern.ircdocs.horse/)
    - [IRC Horse](https://dd.ircdocs.horse/)
    - [Robert Ecker](https://bop.unibe.ch/linguistik-online/article/download/317/467)
* All my friends who helped me do this project


