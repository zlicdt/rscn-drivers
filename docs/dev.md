# How to install driver on Arch Linux

## 识别显卡型号

在安装驱动之前，先确认显卡型号：

```bash
lspci -k | grep -A 2 -E "(VGA|3D)"
```

也可以使用：

```bash
lspci | grep -i vga
```

---

## Intel 显卡驱动

### 适用范围

Intel 集成显卡（HD Graphics / UHD Graphics / Iris / Arc 系列）。

### 安装步骤

**安装驱动包：**

```bash
sudo pacman -S mesa lib32-mesa vulkan-intel lib32-vulkan-intel intel-media-driver
```

| 包名 | 说明 |
|------|------|
| `mesa` | OpenGL 驱动（必装） |
| `lib32-mesa` | 32 位 OpenGL 支持（运行 32 位应用/游戏需要） |
| `vulkan-intel` | Vulkan 驱动 |
| `lib32-vulkan-intel` | 32 位 Vulkan 支持 |
| `intel-media-driver` | 硬件视频加速（Broadwell 及更新架构） |

> 对于较旧的 Intel GPU（GMA 系列），使用 `xf86-video-intel`，但对于较新的 GPU 建议使用内核自带的 modesetting 驱动。

对于 Broadwell 之前的架构，仍需要：

```bash
sudo pacman -S libva-intel-driver
```

---

## AMD 显卡驱动

### 适用范围

AMD / ATI Radeon 显卡（GCN 架构及更新）。

### 开源驱动（推荐）

AMD 的开源驱动 `AMDGPU` 已集成在内核中，性能优秀，推荐使用。

1. **安装驱动包：**

```bash
sudo pacman -S mesa xf86-video-amdgpu vulkan-radeon
```

2. **（可选）AMDGPU PRO：**

如果需要 OpenCL 支持或专业应用，可以从 AUR 安装 `amdgpu-pro` 相关包：

```bash
yay -S amdgpu-pro-libgl opencl-amd
```

### 旧款 ATI 显卡

对于 GCN 之前的旧 ATI 显卡，使用 `xf86-video-ati` 驱动：

```bash
sudo pacman -S xf86-video-ati mesa lib32-mesa
```

---

## NVIDIA 显卡驱动

### 适用范围

NVIDIA GeForce / Quadro / Tesla 系列显卡。

### 专有驱动（推荐）

1. **确定驱动版本：**

| 显卡系列 | 驱动包 |
|----------|--------|
| GeForce 900 系列及更新（Maxwell+） | `nvidia` |
| 使用 LTS 内核 | `nvidia-lts` |
| 使用 DKMS（推荐，兼容所有内核） | `nvidia-dkms` |
| GeForce 700/600 系列（Kepler） | `nvidia-470xx-dkms`（AUR） |

2. **安装驱动（以 DKMS 为例）：**

```bash
sudo pacman -S nvidia-dkms
```

| 包名 | 说明 |
|------|------|
| `nvidia-dkms` | NVIDIA 内核模块（DKMS 版本） |
| `nvidia-utils` | NVIDIA 驱动工具和 OpenGL 库 |
| `lib32-nvidia-utils` | 32 位支持 |
| `nvidia-settings` | 图形化设置工具 |

3. **配置 mkinitcpio：**

编辑 `/etc/mkinitcpio.conf`，删去HOOKS中的`kms`

然后重新生成 initramfs：

```bash
sudo mkinitcpio -P
```

然后重新生成 GRUB 配置：

```bash
sudo grub-mkconfig -o /boot/grub/grub.cfg
```

4. **重启系统：**

```bash
sudo reboot
```

### 开源驱动（Nouveau）

如果不需要高性能 3D 支持，可以使用开源的 Nouveau 驱动：

```bash
sudo pacman -S mesa xf86-video-nouveau
```

> **注意：** Nouveau 驱动性能远不如专有驱动，且不支持较新的 GPU 特性。仅建议在专有驱动无法使用时作为备选。

---

## 验证驱动安装

### 检查 OpenGL

```bash
glxinfo | grep "OpenGL renderer"
```

需安装 `mesa-utils`：

```bash
sudo pacman -S mesa-utils
```

### 检查 Vulkan

```bash
vulkaninfo --summary
```

需安装 `vulkan-tools`：

```bash
sudo pacman -S vulkan-tools
```

### 检查 NVIDIA 驱动状态

```bash
nvidia-smi
```

### 检查内核驱动

```bash
lspci -k | grep -A 3 -E "(VGA|3D)"
```

输出中的 `Kernel driver in use` 字段会显示当前使用的驱动。

---