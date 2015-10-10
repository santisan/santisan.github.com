/**
 * @author Santiago Sanchez - santi.sanchez28@gmail.com
 */

THREE.DistortionPass = function(noiseTex, noiseUvScale, maskTex, distortionScale)
{
    if (THREE.DistortionShader === undefined)
        console.error("THREE.DistortionPass relies on THREE.DistortionShader");    

    var shader = THREE.DistortionShader;

    shader.uniforms.noiseTex.value = noiseTex;
    shader.uniforms.maskTex.value = maskTex;

    if (distortionScale !== undefined && distortionScale !== null) {
        shader.uniforms.distortionScale.value = distortionScale;
    }
    if (noiseUvScale !== undefined && noiseUvScale !== null) {
        shader.uniforms.noiseUvScale.value = noiseUvScale;
    }

    THREE.ShaderPass.call(this, shader, "screenTex");
    shader.uniforms.noiseTex.value.needsUpdate = true;
}

THREE.DistortionPass.prototype = Object.create(THREE.ShaderPass.prototype);
