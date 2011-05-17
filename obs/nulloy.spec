Name:          nulloy
URL:           http://nulloy.com
License:       GPLv3
Summary:       Music player with a waveform progress bar
Version:       0.1.2
Release:       1
Source:        %{name}-%{version}.tar.gz
%if 0%{?fedora_version} || 0%{?suse_version}
BuildRequires: qt-devel gstreamer-devel phonon-devel gcc-c++
%else
BuildRequires: libqt-devel gstreamer-devel libphonon-devel
%endif
BuildRequires: zip
BuildRoot:     %{_topdir}/%{name}-%{version}-root
Patch1: nulloy-fedora.patch

%description
Music player with a waveform progress bar.

%prep
%setup -q
%if 0%{?fedora_version}
%patch1 -p1
%endif

%build
%if 0%{?fedora_version}
%define QMAKE qmake-qt4
%else
%define QMAKE qmake
%endif
%{QMAKE} -config plugins_gstreamer -config plugins_phonon -config silent "PREFIX=%{buildroot}%{_prefix}"
make

%install
%makeinstall

%files
%defattr(-,root,root)
%{_bindir}/%{name}
%{_datadir}/%{name}/*
%exclude %{_datadir}/%{name}/plugins
%{_datadir}/icons/%{name}.png
%{_datadir}/applications/%{name}.desktop

%package  gstreamer
Summary:  GStreamer backend for Nulloy
Requires: nulloy
%description gstreamer
GStreamer backend for Nulloy.
%files gstreamer
%defattr(-,root,root)
%{_libdir}/%{name}/plugins/lib*_gstreamer.so

%package  phonon
Summary:  Phonon backend for Nulloy
Requires: nulloy
%description phonon
Phonon backend for Nulloy.
%files phonon
%defattr(-,root,root)
%{_libdir}/%{name}/plugins/lib*_phonon.so

%changelog
* Fri May 13 2011 Sergey Vlasov <sergey@vlasov.me> 0.1.2-1
- Added files for building in OBS

* Sun May 8 2011 Sergey Vlasov <sergey@vlasov.me> 0.1.1-1
- Shipping GStreamer along with the Windows package

* Thu May 5 2011 Sergey Vlasov <sergey@vlasov.me> 0.1-1
- Initial release
