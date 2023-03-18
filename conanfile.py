from conan import ConanFile
from conan.tools.cmake import cmake_layout


class YetAnotherRISCRecipe(ConanFile):
    name = 'yarisc'
    version = '0.1'
    description = 'Yet Another RISC'
    license = 'MIT'
    author = 'Daniel Eiband'
    
    settings = 'os', 'compiler', 'build_type', 'arch'
    options = {'shared': [True, False], 'fPIC': [True, False]}
    default_options = {'shared': False, 'fPIC': True}
    generators = 'CMakeToolchain', 'CMakeDeps'

    def config_options(self):
        if self.settings.os == 'Windows':
            del self.options.fPIC

    def layout(self):
        cmake_layout(self);

    def requirements(self):
        pass

    def build_requirements(self):
        self.test_requires('catch2/3.3.2')
        self.tool_requires('cmake/3.25.3')
