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
|----------------|-----------------------------------------------------------------------------|
| `/join <#channel1,#chenel2...> <password>`    | Joins the specified channel.                                                 |
| `/part <#channel1,#channel2...>`    | Leaves the specified channel.                                               |
| `/nick <nickname>`    | Changes your nickname to the specified one.                                  |
| `/privmsg <user1,user2...> [message]` | Sends a private message to the specified user.                              |
| `/topic <#channel> or <#channel> [topic]`    | Displays or sets the topic for a channel.                                    |
| `/quit [message]`     | Quits the IRC server with an optional message.                               |
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
|----------------|-----------------------------------------------------------------------------|
| `/mode <#channel> +o or -o <target>`    | Gives or remove operator to a user.                                                 |
| `/mode <#channel> +i or -i`    | Change or remove the channel to Invite Only.                                                 |
| `/mode <#channel> +k or -k <option>`    | Put or remove a password to a channel.                                                 |
| `/mode <#channel> +t or -t`    | Lock or unlock the topic of channel.                                                 |
| `/mode <#channel> +l or -l`    | Set or unset a limit of user to a channel.                                                 |
</div>
<br>
<p>The map can be modified, altered or even created in a simple and functional way, as long as it follows these rules:</p>

```bash
- Only have ONE Player and depending on the letter, which direction will you look when you start the game ([N][S][W][E])
- It has to got a valid RGB value for Floor[F] and Ceiling[C] (No duplicated or invalid RGB value)
- It has to got a valid texture PATH (No duplicated paths or invalid path)
- It has to be enclosed by Walls (1)
- And it needs to be filled with floors (0)
- It can be on any shape, but it HAS to be closed by wall
- The map can only use the following characters: 1, 0, [SPACE], [N,S,W,E] and D -> (D is for Doors only in bonus)
- The map extension must be ".cub"
```
<br>
<br>
<p>Here's a simple example:</p>

```bash
NO assets/sprites/default/north_wall.xpm
SO assets/sprites/default/south_wall.xpm
WE assets/sprites/default/west_wall.xpm
EA assets/sprites/default/east_wall.xpm

F 18,53,25
C 153,204,255

111111111
100000001
1000W0001
100000001
111111111
```
<br>
<br>
<div align="center">
<h3>--------- 42 Rules ---------</h3>
</div>
<br>
<p>In every project we must follow certain rules, here are the rules for this project:</p>

```bash
- The project must not have memory leaks
- The project must use the flags -Wall -Wextra -Werror
- Each function can have a maximum of 25 lines
- Each file should only have 5 functions
- We should keep the code as clean as possible, for example, declarations should be on separate lines
- We cannot use "for", "do...while", "switch", "case", "goto", ternary operators, or variable-length arrays
```

<br>
<br>
<div align="center">

<h3>--------- Credits ---------</h3>
</div>

* My awesome duo in this project: [Bruno](https://github.com/Bruno0798)
* [Textures](https://little-martian.itch.io/retro-texture-pack) and little of handwork of me
* [Ray-Casting]
*   - [Ahmed Fatir](https://medium.com/@afatir.ahmedfatir/cub3d-tutorial-af5dd31d2fcf) (Cub3d Tutorial)
    - [Lode's Computer Graphics Tutorial](https://lodev.org/cgtutor/raycasting.html#The_Basic_Idea_)
* All my friends who helped me do this project


