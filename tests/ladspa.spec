Name:           ladspa
Version:        1.12
Release:        0.fdr.2
Summary:        LADSPA SDK, example plug-ins and tools.
                                                                                
Group:          Libraries/Multimedia
License:        LGPL
URL:            http://www.ladspa.org/
Source:         http://www.ladspa.org/download/%{name}_sdk_%{version}.tgz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
                                                                                
BuildRequires:  perl
BuildRequires:  gcc-c++

%description
There is a large number of synthesis packages in use or development on
the Linux platform at this time. The Linux Audio Developer's Simple
Plugin API (LADSPA) attempts to give programmers the ability to write
simple `plugin' audio processors in C/C++ and link them dynamically
against a range of host applications.
                                                                                
This package contains the example plug-ins and tools from the LADSPA SDK.
                                                                                
%package        devel
Summary:        Linux Audio Developer's Simple Plug-in API 
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description    devel
ladspa-devel contains the ladspa.h header file.
                                                                                
Definitive technical documentation on LADSPA plug-ins for both the host
and plug-in is contained within copious comments within the ladspa.h
header file.

# -----------------------------------------------------------------------------

%prep
%setup -q -n ladspa_sdk
# resspect RPM_OPT_FLAGS
perl -pi -e 's/^(CFLAGS.*)-O3(.*)/$1\$\(RPM_OPT_FLAGS\)$2/' src/makefile

# fix links to the header file in the docs
cd doc
perl -pi -e "s!HREF=\"ladspa.h.txt\"!href=\"file:///usr/include/ladspa.h\"!" *.html

# -----------------------------------------------------------------------------

%build
cd src
make targets %{?_smp_mflags}

#make test
#make check

# -----------------------------------------------------------------------------

%install
rm -rf $RPM_BUILD_ROOT

## ladspa_sdk uses mkdirhier for install which is provided by XFree86
## we don't want to depend on XFree86 for building
## so let's make these dirs ourselves
mkdir -p $RPM_BUILD_ROOT/%{_libdir}/ladspa
mkdir -p $RPM_BUILD_ROOT/%{_includedir}/ladspa
mkdir -p $RPM_BUILD_ROOT/%{_bindir}/ladspa

cd src
make install \
  INSTALL_PLUGINS_DIR=$RPM_BUILD_ROOT/%{_libdir}/ladspa \
  INSTALL_INCLUDE_DIR=$RPM_BUILD_ROOT/%{_includedir} \
  INSTALL_BINARY_DIR=$RPM_BUILD_ROOT/%{_bindir}
                                                                                
# -----------------------------------------------------------------------------

%clean
rm -rf $RPM_BUILD_ROOT

# -----------------------------------------------------------------------------

%files
%defattr(-,root,root,-)
%doc doc/COPYING
%dir %{_libdir}/ladspa
%{_libdir}/ladspa/*.so
%{_bindir}/analyseplugin
%{_bindir}/applyplugin
%{_bindir}/listplugins

%files devel
%defattr(-,root,root,-)
%doc doc/*.html
%{_includedir}/ladspa.h

# -----------------------------------------------------------------------------

%changelog
* Fri Sep 05 2003 Thomas Vander Stichele <thomas at apestaart dot org>
- 1.12-0.fdr.2: fixed RPM_OPT_FLAGS respect

* Thu May 29 2003 Thomas Vander Stichele <thomas at apestaart dot org>
- 1.12-0.fdr.1: initial RPM release
