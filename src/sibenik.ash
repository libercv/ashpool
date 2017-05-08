window 800 600
#window 320 240
#window 1024 768
model models/sibenik/sibenik.obj

gbuffer_shader shaders/gbuffer
lighting_shader shaders/lighting
lighting_kernel kernels/render.cl
//lighting_kernel kernels/render.cl.test_vgpr2

//         position                 znear  zfar     pitch   yaw
camera     -10.0f -12.0f 0.0f        0.1f   100.0f  0.0f  0.0f 

//          speed   zoom   sensitivity
movement    20  45.0f  0.25f

// ambient light intensity
ambient 0.2  
  
//         position                color           diffuse  linear    quadratic
pointlight -5.0f 0.5f 2.0f         1.0f 1.0f 1.0f  1.0f     0.045f   0.0075f
pointlight -15.0f -14.0f 0.0f      1.0f 0.0f 0.0f  1.0f     0.35f     0.44f


