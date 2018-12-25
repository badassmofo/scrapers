#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <thread>
#include <mutex>
#include <termbox.h>

typedef std::uint16_t u16;

typedef struct _cpu {
	u16 mem[0x10000] = {};
	u16 reg[11]      = {};
} cpu_t;

enum reg_index {
  A,
  B,
  C,
  X,
  Y,
  Z,
  I,
  J,
  PC,
  SP,
  EX,
  IA
};

enum ops_index {
	nbi,
	SET,
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,
	SHL,
	SHR,
	AND,
	BOR,
	XOR,
	IFE,
	IFN,
	IFG,
	IFB
};

enum nbi_index {
	JSR = 0x11
};

static bool is_running  = true;
static int  line_offset = 0;
static std::mutex lo_mutex;

auto clip(int n, int low, int high) -> int {
	return std::min(std::max(n, low), high);
}

// Temporary while dumb read file exists
auto swap_int16(int16_t val) -> u16 {
	return (val << 8) | ((val >> 8) & 0xFF);
}

auto print_string(int x, int y, const std::string& s) -> void {
  for (int i = 0; i < s.length(); ++i)
	 tb_change_cell(x + i, y, s[i], TB_WHITE, TB_BLACK);
}

#define HEX_PAD(x) std::setw(x) << std::hex
#define TB_H (tb_height() - 3)

auto dump_mem(cpu_t* cpu) -> void {
  tb_clear();

  std::stringstream s;
  s << std::setfill('0') << " A:  " << HEX_PAD(4) << cpu->reg[A] << " B:  " << HEX_PAD(4) << cpu->reg[B] << " C:  " << HEX_PAD(4) << cpu->reg[C] << " X:  " << HEX_PAD(4) << cpu->reg[X] << " Y: " << HEX_PAD(4) << cpu->reg[Y] << " Z: " << HEX_PAD(4) << cpu->reg[Z] << " I " << HEX_PAD(4) << cpu->reg[I] << " J: " << HEX_PAD(4) << cpu->reg[J];
  print_string(0, 0, s.str());

  s.clear();
	s.str(std::string());
	s << std::setfill('0') << " SP: " << HEX_PAD(4) << cpu->reg[SP] << " PC: " << HEX_PAD(4) << cpu->reg[PC] << " EX: " << HEX_PAD(4) << cpu->reg[EX] << " IA: " << HEX_PAD(4) << cpu->reg[IA];
	print_string(0, 1, s.str());

  for (int i = line_offset * 8; i < (line_offset * 8) + (TB_H * 8); i += 8) {
    std::stringstream ss;
		ss << std::setfill('0') << HEX_PAD(8) << i << "  ";

    for (int j = 0; j < 8; ++j) {
      ss << HEX_PAD(4) << cpu->mem[i + j] << " ";
      if (j == 3)
        ss << " ";
    }

    print_string(0, 3 + (i - (line_offset * 8)) / 8, ss.str());
  }

  tb_present();
}

template<bool skip=false> auto val(u16& v) -> u16& {
}

template<bool skip=false> auto step(cpu_t* cpu) -> void {
  u16 v = cpu->mem[cpu->reg[PC]++];
  u16 o = (v & 0xF);
  u16 a = (v >> 4) & 0x3F;
  u16 b = (v >> 10) & 0x3F;
  
  dump_mem(cpu);
}

auto main(void) -> int {
	tb_init();

	auto cpu = (cpu_t*)malloc(sizeof(cpu_t));
	std::fread(cpu->mem, 0x10000, 2, stdin);
	for (auto& a: cpu->mem)
		a = swap_int16(a);

	dump_mem(cpu);

  auto event_thrd  = std::thread([&]() {
    struct tb_event e;
    while (is_running) {
      tb_poll_event(&e);
      std::lock_guard<std::mutex> lock(lo_mutex);

      switch (e.type) {
  			case TB_EVENT_KEY:
  				switch (e.key) {
  					case TB_KEY_ESC:
  						is_running = false;
  						break;
  					case TB_KEY_ARROW_DOWN:
              line_offset = clip(line_offset - 1, 0, 4096 - (TB_H));
  						break;
  					case TB_KEY_ARROW_UP:
              line_offset = clip(line_offset + 1, 0, 4096 - (TB_H));
  						break;
  					default:
  						break;
  				}
  				break;
  			case TB_EVENT_RESIZE:
  				break;
  			default:
  				break;
  		}
    }
  });

  while (is_running)
    step(cpu);

  event_thrd.join();

  tb_shutdown();
	std::free(cpu);
}
