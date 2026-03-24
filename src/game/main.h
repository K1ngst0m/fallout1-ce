#ifndef FALLOUT_GAME_MAIN_H_
#define FALLOUT_GAME_MAIN_H_

namespace fallout {

extern int main_game_paused;

// Host-callable lifecycle surface used by the Linux desktop adapter and future hosts.
int main_init_system(int argc, char** argv);
int main_run_system();
void main_exit_system();

} // namespace fallout

#endif /* FALLOUT_GAME_MAIN_H_ */
