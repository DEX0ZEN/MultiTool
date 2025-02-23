#!/bin/bash

detect_os() {
    if [[ -f /etc/os-release ]]; then
        . /etc/os-release
        case "$ID" in
            ubuntu|debian|fedora|arch|opensuse*)
                remove_multitool
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

remove_multitool() {
    echo "Removing MultiTool from /usr/local/bin..."
    if sudo rm -f /usr/local/bin/multitool; then
        echo "MultiTool binary removed successfully."
    else
        echo "Failed to remove MultiTool binary."
    fi

    echo "Cleaning up MultiTool repository..."
    if rm -rf ~/MultiTool; then
        echo "MultiTool repository removed successfully."
    else
        echo "Failed to remove MultiTool repository."
    fi

    echo "MultiTool has been completely removed from your system."
}

detect_os
