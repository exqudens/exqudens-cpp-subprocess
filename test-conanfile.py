from pathlib import Path

required_conan_version = ">=2.0"

from conan import ConanFile
from conan.tools.files import copy, save

class ConanConfiguration(ConanFile):
    settings = "arch", "os", "compiler", "build_type"
    options = {"shared": [True, False]}
    default_options = {"shared": True}
    generators = "CMakeDeps"

    def requirements(self):
        try:
            self.requires("gtest/1.11.0")
        except Exception as e:
            self.output.error(e)
            raise e

    def configure(self):
        try:
            self.options["gtest"].shared = self.options.shared
        except Exception as e:
            self.output.error(e)
            raise e

    def generate(self):
        try:
            filename = 'conan-packages.cmake'
            content = ''
            cmake_package_name_property = 'cmake_file_name'

            content += 'set("${PROJECT_NAME}_CONAN_PACKAGE_NAMES"\n'
            for dep in self.dependencies.values():
                content += f'    "{dep.ref.name}"\n'
            content += ')\n'

            content += 'set("${PROJECT_NAME}_CMAKE_PACKAGE_NAMES"\n'
            for dep in self.dependencies.values():
                content += f'    "{dep.cpp_info.get_property(cmake_package_name_property, check_type=str)}" # {dep.ref.name}\n'
            content += ')\n'

            content += 'set("${PROJECT_NAME}_CMAKE_PACKAGE_VERSIONS"\n'
            for dep in self.dependencies.values():
                content += f'    "{dep.ref.version}" # {dep.ref.name}\n'
            content += ')\n'

            content += 'set("${PROJECT_NAME}_CMAKE_PACKAGE_PATHS"\n'
            for dep in self.dependencies.values():
                content += f'    "{Path(dep.package_folder).as_posix()}" # {dep.ref.name}\n'
            content += ')\n'

            save(self, filename, content)

            for dep in self.dependencies.values():
                for dir in dep.cpp_info.bindirs:
                    copy(self, pattern="*.dll", src=Path(dir).as_posix(), dst=Path(self.build_folder).joinpath("bin").as_posix())
        except Exception as e:
            self.output.error(e)
            raise e

if __name__ == "__main__":
    pass
