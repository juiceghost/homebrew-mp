# Skeletton RPM

Summary: Simple yet useful Editor, text and Gtk mode
Name: mp
Version: 3.2.0
Release: 1
Copyright: GPL
Group: System Environment/Libraries
#URL:
Source: %{name}-%{version}.tar.gz
#Source1:
#Patch:
BuildRoot:   /var/tmp/%{name}-%{version}-root


%description

Simple yet useful Editor, text and Gtk mode


%prep
case "${RPM_COMMAND:-all}" in
all)
%setup -q
#%patch -p1
;;
esac


%build

export CFLAGS="$RPM_OPT_FLAGS"
make


%install

mkdir -p $RPM_BUILD_ROOT/usr/bin
make PREFIX=$RPM_BUILD_ROOT/usr install

%clean

rm -rf $RPM_BUILD_ROOT 