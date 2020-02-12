#include "wayland-client.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct global_data
{
  struct wl_compositor *compositor;
  struct wl_shell *shell;
} global_data_t;

/* Registry event listeners. */

void registry_add_object(global_data_t *data,
                         struct wl_registry *registry,
                         uint32_t name, const char *interface,
                         uint32_t version)
{
  if (!strcmp(interface, "wl_compositor"))
  {
    data->compositor = (struct wl_compositor*)wl_registry_bind(
      registry, name, &wl_compositor_interface, 1);
  }
  else if (!strcmp(interface, "wl_shell"))
  {
    data->shell = (struct wl_shell*)wl_registry_bind(
      registry, name, &wl_shell_interface, 1);
  }
}

void registry_remove_object(global_data_t *data,
                            struct wl_registry *registry,
                            uint32_t name)
{
  // TODO
}

struct wl_registry_listener registry_listener = {
  (void(*)(void*, struct wl_registry*, uint32_t, const char*,
           uint32_t))
  &registry_add_object,
  (void(*)(void*, struct wl_registry*, uint32_t))
  &registry_remove_object};

/* Shell event listeners. */

void shell_surface_ping(void *data,
                        struct wl_shell_surface *shell_surface,
                        uint32_t serial)
{
  wl_shell_surface_pong(shell_surface, serial);
}

void shell_surface_popup_done(
  void *data, struct wl_shell_surface *shell_surface)
{

}

void shell_surface_configure(
  void *data, struct wl_shell_surface *shell_surface,
  uint32_t edges, int32_t width, int32_t height)
{

}

struct wl_shell_surface_listener shell_surface_listener = {
  &shell_surface_ping,
  &shell_surface_configure,
  &shell_surface_popup_done};

int main()
{
  /*
   * Open a connection to the Wayland server for a display.
   * Connect to the Wayland display named name. If name is NULL,
   * its value will be replaced with the WAYLAND_DISPLAY
   * environment variable if it is set, otherwise display
   * "wayland-0" will be used.
   */
  const char *display_name = NULL; /* "wayland-0"; */
  struct wl_display *display = wl_display_connect(display_name);

  if (!display)
  {
    fprintf(stderr, "Failed to open Walyand display\n");
    return 1;
  }

  /* Get the registry for the display */
  struct wl_registry *registry =
    wl_display_get_registry(display);

  /* An object that we can pass around to the event listeners. */
  global_data_t data = {NULL, NULL};

  /* Tell Wayland about the registry event listeners. */
  wl_registry_add_listener(registry, &registry_listener, &data);

  /* I assume this flushes command to the server or something? */
  wl_display_roundtrip(display);

  /*
   * registry_add_object should have been called with the
   * compositor alread.
   */
  struct wl_surface *surface =
    wl_compositor_create_surface(data.compositor);

  /*
   * registry_add_object should have been called with the shell
   * alread. Get the surface for the shell.
   */
  struct wl_shell_surface *shell_surface =
    wl_shell_get_shell_surface(data.shell, surface);

  /* Tell Wayland about the shell event listeners. */
  wl_shell_surface_add_listener(shell_surface,
                                &shell_surface_listener,
                                NULL);

  wl_shell_surface_set_toplevel(shell_surface);

  for (;;)
  {
    wl_display_dispatch_pending(display);
  }

  /* Destroy the shell surface. */
  wl_shell_surface_destroy(shell_surface);

  /* Destroy the compositor surface. */
  wl_surface_destroy(surface);

  /* Disconnect from the Wayland display server. */
  wl_display_disconnect(display);
}