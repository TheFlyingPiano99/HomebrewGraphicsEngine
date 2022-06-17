#include "SkyBox.h"
#include<glm/gtc/type_ptr.hpp>

namespace hograengine {

    void SkyBox::draw(FBO& fbo, Camera& camera) {
        fbo.Bind();
        glDepthMask(GL_FALSE);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        shader->Activate();
        glm::mat4 view = glm::mat4(glm::mat3(camera.view)); // Remove translation.
        glm::mat4 viewProj = camera.getProjectionMatrix() * view;
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "camera.viewProjMatrix"), 1, GL_FALSE, glm::value_ptr(viewProj));
        vao.Bind();
        texture.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        fbo.Unbind();
    }
}