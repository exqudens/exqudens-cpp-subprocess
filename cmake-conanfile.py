required_conan_version = ">=2.0"

from conan import ConanFile
from conan.tools.files import copy

class ConanConfiguration(ConanFile):
    def requirements(self):
        try:
            self.requires("exqudens-cmake/2.0.1")
        except Exception as e:
            self.output.error(e)
            raise e

    def generate(self):
        try:
            for dep in self.dependencies.values():
                copy(self, pattern="cmake/util.cmake", src=dep.package_folder, dst=self.build_folder)
        except Exception as e:
            self.output.error(e)
            raise e
