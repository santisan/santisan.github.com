/**
 * @author Santiago Sanchez - santi.sanchez28@gmail.com
 */

THREE.DistortionShader = 
{
    uniforms: 
    {
        distortionScale: { type: "f", value: 0.02 },
        maskTex: { type: "t", value: null },
        noiseTex: { type: "t", value: null },
        screenTex: { type: "t", value: null },        
        noiseUvScale: { type: "v2", value: new THREE.Vector2(1, 1) }
    },

    vertexShader: 
    [
        "uniform vec2 noiseUvScale;"

        "varying vec2 vUv;"
        "varying vec2 vNoiseUv;"

        "void main() {"

            "vUv = uv;"
            "vNoiseUv = noiseUvScale * uv;"
            "gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );"                
        "}"
    ].join("\n"),

    fragmentShader:
    [
        "uniform float distortionScale;"            

        "uniform sampler2D maskTex;"
        "uniform sampler2D noiseTex;"
        "uniform sampler2D screenTex;"

        "varying vec2 vUv;"
        "varying vec2 vNoiseUv;"        

        "void main() {"

        "#ifdef NO_DISTORTION"
            "gl_FragColor = texture2D(screenTex, vUv);"
            "return;"
        "#endif"

            "vec3 mask = texture2D(maskTex, vUv).rgb;"
            "float distortionStrength = min(1.0, dot(mask, mask));"

        "#ifdef VIEW_DISTORTION_MASK"
            "gl_FragColor = vec4(distortionStrength);"
            "return;"
        "#endif"
            
            "vec2 noise = texture2D(noiseTex, vNoiseUv).rg;"
            "noise = noise * distortionScale * distortionStrength;"

            "vec3 distorted = texture2D(screenTex, vUv + noise).rgb;"
            "gl_FragColor = vec4(distorted, 1.0);"            
        }
    ].join("\n")
};
