#  Adapted from awesome https://github.com/Orphis/boost-cmake.git
import os
from conans import ConanFile, CMake, tools, python_requires

base = python_requires("conanpy/1.0@capitis/stable")

class ConanFileImpl(base.ConanCMake):
    name = "wt"
    version = "4.0.5"
  
    def requirements(self):
        self.requires("conancmake/1.17@capitis/stable")
        self.requires("boost/1.70.0@capitis/stable")
        self.requires("win64-openssl/1.0.2k@capitis/stable")
        self.requires("zlib/1.2.11@capitis/stable")

    def build_requirements(self):
        #if self.settings.os == "Windows":
        #   self.build_requires("ToolWin/0.1@user/stable")
        pass

    def system_requirements(self):
        #self.global_system_requirements=True
        #pack_name = None
        #if os_info.linux_distro == "ubuntu":
        #   if os_info.os_version > "12":
        #      pack_name = "package_name_in_ubuntu_10"
        #   else:
        #      pack_name = "package_name_in_ubuntu_12"
        #elif os_info.linux_distro == "fedora" or os_info.linux_distro == "centos":
        #   pack_name = "package_name_in_fedora_and_centos"
        #elif os_info.is_macos:
        #   pack_name = "package_name_in_macos"

        #if pack_name:
        #   installer = SystemPackageTool()
        #   installer.install(pack_name) # Install the package, will update the package database if pack_name isn't already installed
        pass
         
    # def imports(self):
        # self.copy("*.dll", dst="bin", src="bin") # From bin to bin
        # #self.copy("*.dylib*", dst="bin", src="lib") # From lib to bin
