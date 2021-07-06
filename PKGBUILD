
pkgname="desktop-rectangle"
pkgver=1.0
pkgrel=1
arch=('x86_64')
depends=('libx11' 'libxext')
source=('Makefile' 'desktop-rectangle.c')
sha256sums=('SKIP' 'SKIP')

build() {
	make || return 1
}

package() {
	install -D "${pkgname}" "${pkgdir}/usr/bin/${pkgname}"
}
