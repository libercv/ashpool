window 1280 720
model models/sponzacry/sponza.obj

gbuffer_shader shaders/gbuffer
lighting_shader shaders/lighting
lighting_kernel kernels/render.cl


//         position                 znear  zfar     pitch   yaw
camera     -1100.0f 150.0f -40.0f   0.1f   3000.0f  0.0f    0.0f

//          speed   zoom   sensitivity
movement    400.0f  40.0f  0.25f
  
// ambient light intensity
ambient 0.05
  
//         position                color           diffuse  linear    quadratic
//pointlight -680.0f 1900.0f 160.0f  1.0f 1.0f 1.0f  1.0f     0.001f   0.0f
//pointlight -1000.0f 150.0f -350.0f  1.0f 1.0f 1.0f  1.0f     0.001f   0.00008
pointlight 202.90 180.0 466.1      1.0f 1.0f 1.0f  1.0     0.0001f   0.00008
pointlight 602.90 180.0 466.1      1.0f 1.0f 1.0f  1.0     0.0001f   0.00008
pointlight 1002.90 180.0 466.1      1.0f 1.0f 1.0f  1.0      0.0001f   0.00008
pointlight -202.90 180.0 466.1      1.0f 1.0f 1.0f  1.0      0.0001f   0.00008
pointlight -602.90 180.0 466.1      1.0f 1.0f 1.0f  1.0      0.0001f   0.00008
pointlight -1002.90 180.0 466.1      1.0f 1.0f 1.0f  1.0      0.0001f   0.00008

