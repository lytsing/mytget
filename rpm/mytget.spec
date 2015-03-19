##############################################################
# http://baike.corp.taobao.com/index.php/%E6%B7%98%E5%AE%9Drpm%E6%89%93%E5%8C%85%E8%A7%84%E8%8C%83 #
# http://www.rpm.org/max-rpm/ch-rpm-inside.html              #
##############################################################
Name: mytget
Version: 1.12.0
Release: %(echo $RELEASE)%{?dist}
Summary: Myget is a muti-thread downloader accelerator for GNU/Linux.
Group: System Environment/Daemons
Source: %{name}-%{version}.tar.gz
License: Commercial
URL: http://gitlab.alibaba-inc.com/alicdn/Mytget/blob/master/README.md
#BuildRequires: tops-python27, gcc >= 4.2, make >= 3.81
#Requires: package_name = 1.0.0

# disable debuginfo checking
%global debug_package %{nil}

%description
Myget is a muti-thread downloader accelerator for GNU/Linux.

%prep
%setup -q

%build
pwd
cmake .
make

%install
pwd
echo %{buildroot}
mkdir -p %{buildroot}/%{_prefix}
cp -r `pwd`/src/mytget %{buildroot}/%{_prefix}/

%files
%defattr(0755, root, root, 0755)
%{_prefix}/mytget

%pre
#%post -p /sbin/ldconfig
#%postun -p /sbin/ldconfig
%clean

%changelog
* Wed Feb 25 2015 qiushu.zyk
* - add spec of mytget
