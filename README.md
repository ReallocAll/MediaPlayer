# MediaPlayer

[![wakatime](https://wakatime.com/badge/user/2838d0e1-1416-4f45-bc46-cbda8f4d9e75/project/193328a5-c16a-4ad4-9ab2-f18b70349042.svg)](https://wakatime.com/badge/user/2838d0e1-1416-4f45-bc46-cbda8f4d9e75/project/193328a5-c16a-4ad4-9ab2-f18b70349042)

[![CodeFactor](https://www.codefactor.io/repository/github/willowsaucer/mediaplayer/badge)](https://www.codefactor.io/repository/github/willowsaucer/mediaplayer)

English | [简体中文](README_ZH.md)

## Introduction
This project uses the [LittleHooker](https://github.com/WillowSauceR/LittleHooker) framework for HOOK.

## Introduction
This plugin is used to play NBS music on the server.

## Usage
NBS music files should be placed in the ``plugins\MediaPlayer\nbs`` folder
Use ``/mp`` or ``/mediaplayer`` as the main command in the game
The subcommands are ``list``, ``play`` and ``stop``.

| subcommands | parameters | examples   | explanation    |
| ----------- | ---------- | ---------- | -------------- |
| list        | none       | /mp list   | Get song index |
| play        | song index | /mp play 1 | play song      |
| stop        | none       | /mp stop   | stop playing   |

Note: Before playing music, please use ``/mp list`` to get the music list first.
You can also use the ``/mp`` command in-game to see how to use it

## Recommended NBS music resources
It's not easy to make original nbs music ~ come and support them!
[Original nbs music collection](https://www.minebbs.com/resources/nbs.4773/)
[nbs music "water"](https://www.minebbs.com/resources/nbs-water.4365/)
[nbs music collection](https://github.com/nickg2/NBSsongs)

## TODO
Download NBS music for playback within the game using the command input URL
