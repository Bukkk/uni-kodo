src = include_directories('../src')

args_test = executable('args',
    'args.cpp',
  
    include_directories : src
)

test('test modulu parsowania argumentow', args_test,
    args: ['blah1', 'blah2'])


elias_gamma_test = executable('gamma',
    'elias_gamma.cpp',
  
    include_directories : src
)

test('kodowanie gamma eliasa', elias_gamma_test)

elias_delta_test = executable('delta',
    'elias_delta.cpp',
  
    include_directories : src
)

test('kodowanie delta eliasa', elias_delta_test)

elias_omega_test = executable('omega',
    'elias_omega.cpp',
  
    include_directories : src
)

test('kodowanie omega eliasa', elias_omega_test)