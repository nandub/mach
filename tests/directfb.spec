Name:           directfb
Version:        0.9.19
Release:        0.fdr.2
Summary:        Graphics abstraction library for the Linux Framebuffer Device.

Group:          System/Libraries
License:        GPL
URL:            http://www.directfb.org/
Source:         http://www.directfb.org/download/DirectFB/DirectFB-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  libpng-devel
BuildRequires:  zlib-devel
BuildRequires:  libjpeg-devel
BuildRequires:  freetype-devel
BuildRequires:	SDL-devel
# the libtool in the tarball has a problem with installing binaries linked
# against the included library
BuildRequires:	libtool

%define oname	DirectFB

%description
DirectFB is a thin library that provides hardware graphics acceleration,
input device handling and abstraction, integrated windowing system with
support for translucent windows and multiple display layers on top of the
Linux Framebuffer Device.

It is a complete hardware abstraction layer with software fallbacks for 
every graphics operation that is not supported by the underlying hardware.
DirectFB adds graphical power to embedded systems and sets a new standard 
for graphics under Linux. 

%package        devel
Summary:        Development package for %{name}
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description    devel
The %{name}-devel package contains the static libraries and header files
needed for development with %{name}.

# -----------------------------------------------------------------------------

%prep
%setup -q -n %{oname}-%{version}
#%patch -p1

# -----------------------------------------------------------------------------

%build
%configure \
        --disable-maintainer-mode \
        --enable-shared \
        --enable-static \
        --enable-fast-install \
        --disable-avifile \
        --disable-debug

# ugly hack, seems to work to make sure binaries get relinked properly
# cp /usr/bin/libtool .

make %{?_smp_mflags}

# -----------------------------------------------------------------------------

%install
rm -rf $RPM_BUILD_ROOT
cd src
make DESTDIR=$RPM_BUILD_ROOT install
cd ..
make DESTDIR=$RPM_BUILD_ROOT install

find $RPM_BUILD_ROOT -type f -name "*.la" -exec rm -f {} ';'

# -----------------------------------------------------------------------------

%clean
rm -rf $RPM_BUILD_ROOT

# -----------------------------------------------------------------------------

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

# -----------------------------------------------------------------------------

%files
%defattr(-,root,root,-)
%doc AUTHORS COPYING ChangeLog NEWS README TODO
%{_libdir}/libdirectfb-*.so.*
%{_libdir}/directfb-%{version}
%{_datadir}/%{name}-%{version}
%{_bindir}/dfbg
%{_bindir}/dfbdump
%{_bindir}/dfbinfo
%{_bindir}/dfblayer
%{_mandir}/man1/dfbg.1*
%{_mandir}/man5/directfbrc.5*

%files devel
%defattr(-,root,root,-)
%doc docs/html/*
%{_bindir}/directfb-config
%{_bindir}/directfb-csource
%{_includedir}/directfb
%{_includedir}/directfb-internal
%{_mandir}/man1/directfb-csource.1*
%{_libdir}/pkgconfig/directfb.pc
%{_libdir}/pkgconfig/directfb-internal.pc
%{_libdir}/libdirectfb.so
%{_libdir}/libdirectfb.a

# -----------------------------------------------------------------------------

%changelog
* Tue Aug 19 2003 Thomas Vander Stichele <thomas at apestaart dot org>
- 0.9.18-0.fdr.2:
  - incorporated Anvil's suggestions

* Sun Jul 06 2003 Thomas Vander Stichele <thomas at apestaart dot org>
- 0.9.18-0.fdr.1: initial rpm release
