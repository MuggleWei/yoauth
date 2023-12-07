# YoAuth
## Overview
**YoAuth** is a local, pure terminal command line TOTP generator, used for 2FA verification of accounts such as github, google or microsoft  

* [readme EN](./readme.md)
* [readme 中文](./readme_cn.md)

## Vedio showcase
* [BiliBili](https://www.bilibili.com/video/BV1G14y1k7Gr)
* [Youtube](https://www.youtube.com/watch?v=273TW5fSyVg)

## Download/Compile

### Download
You can directly download the corresponding package from [Release](https://github.com/MuggleWei/yoauth/releases) of this project, depressed and use it directly  

### Compile From Source Code
Users can also compile from the source code without download package. There are many options to choose
* Directly use the script compilation in the project
* Use the C package management system to download dependencies and compile

For the sake of simplicity, users who are not familiar with C package management can also easily to compile, here only introduces the way of directly using build script in project  

#### Dependency tools
Gurantee Cmake and git already installed

#### Install OpenSSL (Optional && Not recommended)
This step is optional. By Default, **YoAuth** will download and compile `openssl`, the advantage of this is that it can generate a `portable` package, it can be directly copied and used on the same system; the disadvantage is that the compilation step will take more time.  

User also can choose to use system package management to install `openssl`  

* close openssl build  
Modify `build.sh` in the project root directory, find `build_openssl=1` and change it to `build_openssl=0`

* ubuntu
```
sudo apt install openssl libssl-dev
```

* arch linux
```
sudo pacman -S openssl
```

#### Compile
Run `build.sh` in the root directory of the project, after running, `dist/yoauth*.tar.gz` will be generated in the root directory

## Usage

### First Use
For the first use, run the `yoauth` command directly, you will be prompted to enter the password twice, and an encrypted file storing the TOTP key will be generated locally. When no other options are entered, the default folder name will be used. Because all content of **YoAuth** is stored locally, please be sure to remember the password you entered, once lost, it will not be retrieved  

### Add Account And Key
For example, the 2FA verification of `github` needs to be added currently, assuming the key is: `NBSWY3DPO5XXE3DE`
```
yoauth add -a github -k NBSWY3DPO5XXE3DE
```

### Delete Account
For example, currently you need to delete the account key of `github`
```
yoauth delete -a github
```

### View Verification Code
All verification codes need to be listed, just run `yoauth` directly  
When you need to filter accounts, you can use the `list` command. For example, you currently want to filter out the verification codes that contain the `google` string in the account.
```
yoauth list -f google
```

## Backup/Migration
When you need to back up/migrate keys, there are two methods:

### Backup Data Files
By default, the file storing the key is in the `data` folder of the user's decompressed directory, and the user can directly copy it to other places for backup

### Dump
Users can also export all keys into plaintext for migration
```
yoauth dump -o ./backup.txt
```
