# YoAuth
## 概述
**YoAuth** 是一个本地的，纯命令行终端下的 TOTP generator，可用于诸如 github, google 或者 microsoft 账户的 2FA 验证

* [readme EN](./readme.md)
* [readme 中文](./readme_cn.md)

## 视频演示
* [BiliBili](https://www.bilibili.com/video/BV1G14y1k7Gr)
* [Youtube](https://www.youtube.com/watch?v=273TW5fSyVg)

## 下载/编译

### 下载
可以直接从此项目的 [Release](https://github.com/MuggleWei/yoauth/releases) 下载对应的包，解压并直接使用

### 从源码编译
用户也可以不使用打好的包，直接从源码进行编译，这里有多种可选择的方案
* 直接使用项目中的脚本编译
* 使用 C 包管理系统下载依赖项并编译

简单起见，为了不熟悉 C 包管理的用户也可以轻松编译，这里仅介绍直接使用使用项目中脚本进行编译的方式

#### 依赖工具
确保 CMake 和 git 都已经安装

#### 安装 openssl (可选)
此步骤是可选的, 默认情况下, **YoAuth** 会下载并编译 `openssl`, 这样的好处是可以打出一个 `portable` 安装包，确保了在相同系统中可以直接拷贝使用; 劣势是编译步骤将耗费较多的时间  

用户也可以选择使用系统包管理安装

* 关闭 openssl 编译
修改项目根目录中的 `build.sh`, 在其中找到 `build_openssl=1` 并将其改为 `build_openssl=0`

* ubuntu
```
sudo apt install openssl libssl-dev
```

* arch linux
```
sudo pacman -S openssl
```

#### 编译
运行项目根目录中的 `build.sh`，运行结束后，将在根目录中生成 `dist/yoauth*.tar.gz`

## 使用
将下载/编译好的包解压到任何你想要的位置，比如: `./local/app/yoauth`，运行其 `bin` 目录中 `yoauth -h`，将看到使用帮助  

### 第一次使用
第一次使用，可以直接运行 `yoauth` 命令，会提示你输入两次密码，在本地生成一个加密的存储 TOTP 密钥的文件，当没用输入其他选项时，会使用默认的文件夹名称。因为 **YoAuth** 的所有内容均为本地存储，请务必记住输入的密码，一旦丢失，将无法找回  

### 添加账户和密钥
例如，当前需要添加 `github` 的 2FA 验证，假设密钥为: `NBSWY3DPO5XXE3DE`
```
yoauth add -a github -k NBSWY3DPO5XXE3DE
```

### 删除账户
例如，当前需要删除 `github` 的账户密钥
```
yoauth delete -a github
```

### 查看验证码
需要列出所有验证码，直接运行 `yoauth` 即可  
当需要对账户进行筛选时，可以使用 `list` 命令，例如当前想要筛选得到账户中包含 `google` 字符串的验证码
```
yoauth list -f google
```

## 备份/迁移
当需要将密钥备份/迁移有两种方法

### 备份 data 文件
默认情况下，存储密钥的文件就在用户所解压目录的 `data` 文件夹中，用户可以直接拷贝到其他地方进行备份

### Dump
用户也可以将所有密钥导出成明文形式进行迁移
```
yoauth dump -o ./backup.txt
```
