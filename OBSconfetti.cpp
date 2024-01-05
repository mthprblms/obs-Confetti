#include <obs-module.h>
#include <GLFW/glfw3.h>

struct ConfettiSource {
    uint32_t particle_size;
    uint32_t particle_density;
    GLFWwindow *window;
};

static void draw_confetti_particles(uint32_t width, uint32_t height,
                                    uint32_t particle_size, uint32_t particle_density) {
    glClear(GL_COLOR_BUFFER_BIT);

    for (uint32_t i = 0; i < particle_density; ++i) {
        float x = static_cast<float>(rand() % width);
        float y = static_cast<float>(rand() % height);
        float size = static_cast<float>(particle_size);

        glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 1.0f); // White color for simplicity
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y + size);
        glEnd();
    }

    glfwSwapBuffers(glfwGetCurrentContext());
}

static void confetti_explode(obs_source_t *source, uint32_t width, uint32_t height,
                              uint32_t particle_size, uint32_t particle_density) {
    struct ConfettiSource *confetti = (struct ConfettiSource *)obs_source_get_settings(source);
    draw_confetti_particles(width, height, particle_size, particle_density);
}

static void confetti_source_update(void *data, obs_data_t *settings) {
    struct ConfettiSource *confetti = (struct ConfettiSource *)data;
    confetti->particle_size = (uint32_t)obs_data_get_int(settings, "particle_size");
    confetti->particle_density = (uint32_t)obs_data_get_int(settings, "particle_density");
}

static obs_properties_t *confetti_properties(void *data) {
    obs_properties_t *props = obs_properties_create();

    obs_properties_add_int(props, "particle_size", "Particle Size", 1, 100, 1);
    obs_properties_add_int(props, "particle_density", "Particle Density", 1, 100, 1);

    return props;
}

static void confetti_defaults(obs_data_t *settings) {
    obs_data_set_default_int(settings, "particle_size", 10);
    obs_data_set_default_int(settings, "particle_density", 50);
}

static const char *confetti_getname(void *unused) {
    return "Confetti Exploder";
}

static void *confetti_create(obs_data_t *settings, obs_source_t *source) {
    struct ConfettiSource *confetti = (struct ConfettiSource *)bzalloc(sizeof(struct ConfettiSource));

    if (glfwInit() != GLFW_TRUE) {
        blog(LOG_ERROR, "Failed to initialize GLFW");
        return nullptr;
    }

    confetti->window = glfwCreateWindow(1920, 1080, "Confetti", nullptr, nullptr);
    if (!confetti->window) {
        blog(LOG_ERROR, "Failed to create GLFW window");
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(confetti->window);
    glewInit();

    confetti_source_update(confetti, settings);

    return confetti;
}

static void confetti_destroy(void *data) {
    struct ConfettiSource *confetti = (struct ConfettiSource *)data;

    glfwDestroyWindow(confetti->window);
    glfwTerminate();

    bfree(data);
}

static void confetti_tick(void *data, float seconds) {
    struct ConfettiSource *confetti = (struct ConfettiSource *)data;

    int width, height;
    glfwGetFramebufferSize(confetti->window, &width, &height);

    confetti_explode(nullptr, width, height, confetti->particle_size, confetti->particle_density);
    glfwPollEvents();

    UNUSED_PARAMETER(seconds);
}

struct obs_source_info confetti_source_info = {
    .id = "confetti_exploder",
    .type = OBS_SOURCE_TYPE_INPUT,
    .output_flags = OBS_SOURCE_VIDEO,
    .get_name = confetti_getname,
    .create = confetti_create,
    .destroy = confetti_destroy,
    .update = confetti_source_update,
    .get_properties = confetti_properties,
    .get_defaults = confetti_defaults,
    .tick = confetti_tick,
};

bool obs_module_load(void) {
    obs_register_source(&confetti_source_info);
    return true;
}
