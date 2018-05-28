%{!?_topdir: %define _topdir %(pwd)}

Name: cligen
Version: %{_version}
Release: %{_release}
Summary: The CLIGEN command line generation tool
Group: System Environment/Libraries
License: ASL 2.0 or GPLv2
URL: http://www.cligen.se
BuildRequires: flex, bison

Source: %{name}-%{version}-%{release}.tar.xz

%description
The CLIGEN command line generation tool.

%package devel
Summary: CLIGEN header files
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
This package contains header files for CLIGEN.

%prep
%setup

%build
%configure
make

%install
make DESTDIR=${RPM_BUILD_ROOT} install

%files
%{_libdir}/lib*

%files devel
%{_includedir}/%{name}/*

%clean

%post
/sbin/ldconfig

%postun
/sbin/ldconfig
