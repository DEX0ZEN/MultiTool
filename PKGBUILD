pkgname=multitool
pkgver=1.0
pkgrel=1
pkgdesc="A multi-functional terminal utility for directory management and system tasks"
arch=('x86_64')
url="https://github.com/yourusername/multitool"
license=('GPL3')
depends=('gcc-libs')
makedepends=('gcc' 'make')
source=("multitool.cpp")
sha256sums=('SKIP')

build() {
  cd "$srcdir"
  g++ -std=c++17 -o multitool src/MultiTool.cpp -lstdc++fs
}

package() {
  install -Dm755 "$srcdir/multitool" "$pkgdir/usr/bin/multitool"
}