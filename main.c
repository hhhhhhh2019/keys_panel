#include <xcb/xcb.h>
#include <X11/XF86keysym.h>
#include <xcb/xtest.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define rgb(r,g,b) (b | (g << 8) | (r << 16))


xcb_point_t back[3] = {
	{336+720/3+24-16,19},
	{336+720/3-24-16,43},
	{336+720/3+24-16,67}
};

xcb_arc_t home[1] = {
	{336,19, 48,48, 0,360<<6}
};

xcb_rectangle_t menu[1] = {
	{120+16,19, 48,48}
};


unsigned int strut[4] = {0, 0, 0, 86};
unsigned int strut_partial[12] = {0, 0, 0, 86, 1280-86, 1280, 1280-86, 1280, 0, 720, 0, 720};

int main() {
	xcb_connection_t* connection = xcb_connect(NULL, NULL);

	const xcb_setup_t*    setup  = xcb_get_setup(connection);
	xcb_screen_iterator_t iter   = xcb_setup_roots_iterator(setup);
	xcb_screen_t*         screen = iter.data;


	xcb_gcontext_t gc	  = xcb_generate_id(connection);
	uint32_t	   mask	  = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
	uint32_t	   values[2] = {rgb(200,200,200), 0};

	xcb_create_gc (connection, gc, screen->root, mask, values);


	xcb_window_t window = xcb_generate_id(connection);

	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = rgb(30,30,30);
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS;

	xcb_create_window(connection,
			XCB_COPY_FROM_PARENT,
			window,
			screen->root,
			0, 1280-86,
			720, 86,
			0,
			XCB_WINDOW_CLASS_INPUT_OUTPUT,
			screen->root_visual,
			mask, values);


	xcb_intern_atom_cookie_t window_type_atom_cookie = xcb_intern_atom(connection, 0, strlen("_NET_WM_WINDOW_TYPE"), "_NET_WM_WINDOW_TYPE");
	xcb_intern_atom_reply_t* window_type_atom_reply = xcb_intern_atom_reply(connection, window_type_atom_cookie, NULL);

	xcb_intern_atom_cookie_t window_type_dock_cookie = xcb_intern_atom(connection, 0, strlen("_NET_WM_WINDOW_TYPE_DOCK"), "_NET_WM_WINDOW_TYPE_DOCK");
	xcb_intern_atom_reply_t* window_type_dock_reply = xcb_intern_atom_reply(connection, window_type_dock_cookie, NULL);

	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, window_type_atom_reply->atom, XCB_ATOM_ATOM, 32, 1, &window_type_dock_reply->atom);


	xcb_intern_atom_cookie_t window_state_atom_cookie = xcb_intern_atom(connection, 0, strlen("_NET_WM_STATE"), "_NET_WM_STATE");
	xcb_intern_atom_reply_t* window_state_atom_reply = xcb_intern_atom_reply(connection, window_state_atom_cookie, NULL);

	xcb_intern_atom_cookie_t window_state_sticky_cookie = xcb_intern_atom(connection, 0, strlen("_NET_WM_STATE_STICKY"), "_NET_WM_STATE_STICKY");
	xcb_intern_atom_reply_t* window_state_sticky_reply = xcb_intern_atom_reply(connection, window_state_sticky_cookie, NULL);

	xcb_intern_atom_cookie_t window_state_above_cookie = xcb_intern_atom(connection, 0, strlen("_NET_WM_STATE_ABOVE"), "_NET_WM_STATE_ABOVE");
	xcb_intern_atom_reply_t* window_state_above_reply = xcb_intern_atom_reply(connection, window_state_above_cookie, NULL);

	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, window_state_atom_reply->atom, XCB_ATOM_ATOM, 32, 1, &window_state_sticky_reply->atom);
	xcb_change_property(connection, XCB_PROP_MODE_APPEND, window, window_state_atom_reply->atom, XCB_ATOM_ATOM, 32, 1, &window_state_above_reply->atom);


	xcb_intern_atom_cookie_t window_class_atom_cookie = xcb_intern_atom(connection, 0, strlen("WM_CLASS"), "WM_CLASS");
	xcb_intern_atom_reply_t* window_class_atom_reply = xcb_intern_atom_reply(connection, window_class_atom_cookie, NULL);

	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, window_class_atom_reply->atom, XCB_ATOM_STRING, 8, strlen("KeysPanel"), "KeysPanel");


	xcb_intern_atom_cookie_t window_strut_atom_cookie = xcb_intern_atom(connection, 0, strlen("_NET_WM_STRUT"), "_NET_WM_STRUT");
	xcb_intern_atom_reply_t* window_strut_atom_reply = xcb_intern_atom_reply(connection, window_strut_atom_cookie, NULL);

	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, window_strut_atom_reply->atom, XCB_ATOM_CARDINAL, 32, 4, &strut);


	xcb_intern_atom_cookie_t window_strut_partial_atom_cookie = xcb_intern_atom(connection, 0, strlen("_NET_WM_STRUT_PARTIAL"), "_NET_WM_STRUT_PARTIAL");
	xcb_intern_atom_reply_t* window_strut_partial_atom_reply = xcb_intern_atom_reply(connection, window_strut_partial_atom_cookie, NULL);

	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, window_strut_partial_atom_reply->atom, XCB_ATOM_CARDINAL, 32, 12, &strut_partial);


	xcb_configure_window(connection, window, 0, NULL);

	xcb_map_window(connection, window);
	xcb_flush(connection);


	xcb_generic_event_t *event;
	while ((event = xcb_wait_for_event(connection))) {
		switch (event->response_type & ~0x80) {
			case XCB_EXPOSE:
				xcb_fill_poly(connection, window, gc, XCB_POLY_SHAPE_NONCONVEX, 0, 3, back);
				xcb_poly_fill_arc(connection, window, gc, 1, home);
				xcb_poly_fill_rectangle(connection, window, gc, 1, menu);

				xcb_flush(connection);

				break;
			case XCB_BUTTON_PRESS:
				xcb_button_press_event_t* press = (xcb_button_press_event_t*)event;

				//printf("%d %d\n", press->event_x, press->event_y);

				if (menu[0].x - 48 + 24 < press->event_x && press->event_x < menu[0].x + 48 + 24) {
				}

				if (home[0].x - 48 + 24 < press->event_x && press->event_x < home[0].x + 48 + 24) {
					//xcb_test_fake_input(connection, XCB_KEY_PRESS, XF86XK_HomePage, XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0);
					//xcb_test_fake_input(connection, XCB_KEY_RELEASE, XF86XK_HomePage, XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0);

					system("xdotool key XF86HomePage"); // так надо
				}

				if (back[0].x - 48*2 < press->event_x && press->event_x < back[1].x + 48*2) {
					/*xcb_test_fake_input(connection, XCB_KEY_PRESS, XF86XK_Back, XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0);
					xcb_test_fake_input(connection, XCB_KEY_RELEASE, XF86XK_Back, XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0);*/

					/*xcb_key_press_event_t e;

					e.event = window;
					e.time = XCB_CURRENT_TIME;
					e.root = screen->root;
					e.detail = XF86XK_Back;

					xcb_send_event(connection, 0, screen->root, XCB_EVENT_MASK_KEY_PRESS, (char *)&e);

					xcb_flush(connection);*/

					system("xdotool key XF86Back");
				}

				break;
			default:
				break;
			}

			free(event);
	}

	xcb_disconnect(connection);
}
