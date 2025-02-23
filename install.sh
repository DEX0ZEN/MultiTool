#!/bin/bash

detect_os() {
    if [[ -f /etc/os-release ]]; then
        . /etc/os-release
        case "$ID" in
            ubuntu|debian)
                install_deb
                ;;
            fedora)
                install_fedora
                ;;
            arch)
                install_arch
                ;;
            opensuse*)
                install_opensuse
                ;;
            *)
                echo "Unsupported OS: $ID"
                exit 1
                ;;
        esac
    else
        echo "Cannot detect OS. Unsupported system."
        exit 1
    fi
}

install_deb() {
    echo "Installing dependencies for Debian/Ubuntu..."
    sudo apt update && sudo apt install -y --no-install-recommends g++ git
    install_multitool
}

install_fedora() {
    echo "Installing dependencies for Fedora..."
    sudo dnf install -y g++ git
    install_multitool
}

install_arch() {
    echo "Installing dependencies for Arch Linux..."
    sudo pacman -Sy --needed --noconfirm base-devel git
    install_multitool
}

install_opensuse() {
    echo "Installing dependencies for openSUSE..."
    sudo zypper install -y gcc-c++ git
    install_multitool
}

install_multitool() {
    echo "Cloning MultiTool repository..."
    git clone https://github.com/DEX0ZEN/MultiTool.git ~/MultiTool
    cd ~/MultiTool || exit 1

    echo "Compiling MultiTool..."
    g++ -std=c++17 -o multitool MultiTool.cpp -lstdc++fs

    echo "Installing MultiTool to /usr/local/bin..."
    sudo mv multitool /usr/local/bin/
    sudo chmod +x /usr/local/bin/multitool

    echo "MultiTool installed successfully! You can now run 'multitool' from anywhere."
}

detect_os
