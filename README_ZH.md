# MediaPlayer

[![wakatime](https://wakatime.com/badge/user/2838d0e1-1416-4f45-bc46-cbda8f4d9e75/project/193328a5-c16a-4ad4-9ab2-f18b70349042.svg)](https://wakatime.com/badge/user/2838d0e1-1416-4f45-bc46-cbda8f4d9e75/project/193328a5-c16a-4ad4-9ab2-f18b70349042)

[![CodeFactor](https://www.codefactor.io/repository/github/willowsaucer/mediaplayer/badge)](https://www.codefactor.io/repository/github/willowsaucer/mediaplayer)

[English](README.md) | 简体中文

本项目使用了[LittleHooker](https://github.com/WillowSauceR/LittleHooker)框架进行HOOK。

## 简介
本插件用于在BDS服务器播放NBS音乐。

## 使用
NBS音乐文件请放置到``plugins\MediaPlayer\nbs``文件夹中
注意：目前暂不支持使用除英文以外语言作为文件名的NBS音乐，请在使用将其文件名改为英文。文件中允许使用空格，下划线，数字，英文字母等

在游戏中使用``/mp``或``/mediaplayer``作为主命令
子命令为``list``，``play``和``stop``。

| 子命令 | 参数     | 示例       | 解释         |
| ------ | -------- | ---------- | ------------ |
| list   | 无       | /mp list   | 获取歌曲索引 |
| play   | 歌曲索引 | /mp play 1 | 播放歌曲     |
| stop   | 无       | /mp stop   | 停止播放     |

注意：在播放音乐之前，请先使用``/mp list``来获取音乐列表。
具体使用方法，也可以在游戏内使用``/mp``命令查看

## 推荐NBS音乐资源
制作原创nbs音乐可不容易呢~来支持一下他们吧

[原创nbs音乐集合](https://www.minebbs.com/resources/nbs.4773/)

[nbs音乐《water》](https://www.minebbs.com/resources/nbs-water.4365/)

[nbs音乐集合](https://github.com/nickg2/NBSsongs)

## 待办
在游戏内利用命令输入URL下载NBS音乐进行播放

## 致谢

- [yqs112358](https://github.com/yqs112358) 提供了在发包层面的技术支持
- [lgc2333](https://github.com/lgc2333) 参与了[nbsparser](https://github.com/WillowSauceR/nbsparser)的开发，提供了基础技术支持

没有他们的支持，这个项目不可能实现

## 许可证

| 项目                                                         | 许可证                                                                   |
| ------------------------------------------------------------ | ------------------------------------------------------------------------ |
| [LittleHooker](https://github.com/WillowSauceR/LittleHooker) | GPL-3.0                                                                  |
| [nbsparser](https://github.com/WillowSauceR/nbsparser)       | GPL-3.0                                                                  |
| [minhook](https://github.com/TsudaKageyu/minhook)            | [许可证](https://github.com/TsudaKageyu/minhook/blob/master/LICENSE.txt) |
| [uthash](https://github.com/troydhanson/uthash)              | [许可证](https://github.com/troydhanson/uthash/blob/master/LICENSE)      |
| [microsoft-pdb](https://github.com/microsoft/microsoft-pdb)  | [许可证](https://github.com/microsoft/microsoft-pdb/blob/master/LICENSE) |
| [Horion](https://github.com/horionclient/Horion)             | [许可证](https://github.com/horionclient/Horion/blob/master/LICENSE)     |
