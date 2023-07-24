project('rt-cpu',
    'cpp',
    version: '0.0.1',
    default_options: [
        'warning_level=3',
        'cpp_std=gnu++20'
    ],
)

cpp = meson.get_compiler('cpp')

sources = []

subdir('src')

executable('rt-cpu',
    sources,
    include_directories: [
        'src'
    ],
    install: false,
)