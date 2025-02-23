pkgname=multitool
pkgver=1.0
pkgrel=1
pkgdesc="A simple multitool program"
arch=('x86_64')
url="https://github.com/DEX0ZEN/MultiTool"
license=('MIT') 
depends=('gcc') 
source=("MultiTool.cpp")
sha256sums=('SKIP')

build() {
    cd "$srcdir"
    g++ -std=c++17 -o multitool MultiTool.cpp -lstdc++fs
}

package() {
    install -Dm755 "$srcdir/multitool" "$pkgdir/usr/bin/multitool"
}
