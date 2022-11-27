#include <cstdint>
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>
using namespace std; // fuck you and your code
namespace jls {
    // declarations
    using ubyte = uint8_t;
    using uint=uint32_t;
    using ull = uint64_t;
    using tiii = tuple<int,int,int>;
    using tbbb = tuple<char,char,char>;
    string int_to_str_div_1M(int);
    ull three_pack(int, int, int);
    ull three_pack(tiii);
    tiii three_unpack(ull);
    void push_int24(vector<ubyte>&, int);
    void push_int(vector<ubyte>&, int);
    void push_int64(vector<ubyte> &v, ull);
    struct node_v;
    struct node_vt;
    struct node_vn;
    struct node_f;
    struct delta_f;
    class fixed_byte_stream {
    protected:
        unsigned char* vstr;
        int ptr;
        int maxSize;
    public:
        fixed_byte_stream(unsigned char *buf, int length) {
            vstr = buf;
            ptr = 0;
            maxSize = length;
        }
        int get_remaining_size() const {
            return maxSize - ptr;
        }
        void clear() {
            ptr = 0;
        }
        friend fixed_byte_stream& operator >> (fixed_byte_stream&, char &b);
        friend fixed_byte_stream& operator >> (fixed_byte_stream&, ubyte &b);
        friend fixed_byte_stream& operator >> (fixed_byte_stream&, int &x);
        friend fixed_byte_stream& operator >> (fixed_byte_stream&, ull &x);
        friend fixed_byte_stream& operator >> (fixed_byte_stream&, node_v &v);
        friend fixed_byte_stream& operator >> (fixed_byte_stream&, node_vt &vt);
        friend fixed_byte_stream& operator >> (fixed_byte_stream&, node_vn &vn);
        friend fixed_byte_stream& operator >> (fixed_byte_stream&, node_f &f);
        friend fixed_byte_stream& operator >> (fixed_byte_stream&, delta_f &df);
        int get_int24() {
            int v = 0;
            for (int i = 0; i < 3; i++) {
                v <<= 8;
                v |= vstr[ptr++];
            }
            return v;
        }
    };
    const int FLAG_BYTES = 1;
    const ubyte FLAGS[] = {
        0x00
    };
    const int NUM_BLOCKS = 4;
    const unsigned int msk_pack = 0x1fffff;
 
 
    // Implementations
    string int_to_str_div_1M(int x) {
        string ret("");
        bool is_neg = (x < 0);
        if (is_neg) { x = -x; }
        while (x) {
            int m = x % 10;
            ret += char(m + '0');
            x /= 10;
        }
        while (ret.size() < 7) {
            ret += "0";
        }
        ret.insert(6, ".");
        if (is_neg) {
            ret += '-';
        }
        reverse(ret.begin(), ret.end());
        return ret;
    }
    int write_pos_int_less_than_10M(char *buf, int x) {
        int offset = 0;
        if (x >= 1000000) { buf[offset++] = (x / 1000000) % 10 + '0'; }
        if (x >= 100000)  { buf[offset++] = (x / 100000) % 10 + '0'; }
        if (x >= 10000)   { buf[offset++] = (x / 10000) % 10 + '0'; }
        if (x >= 1000)    { buf[offset++] = (x / 1000) % 10 + '0'; }
        if (x >= 100)     { buf[offset++] = (x / 100) % 10 + '0'; }
        if (x >= 10)      { buf[offset++] = (x / 10) % 10 + '0'; }
        buf[offset++] = (x % 10) + '0';
        return offset;
    }
    int write_int_to_str_div_1M(char *buf, int x) {
        int offset = 0;
        if (x == 0) {
            buf[offset] = '0';
            return 1;
        }
        if (x < 0) {
            buf[offset++] = '-';
            x = -x;
        }
        if (x >= 1000000000) {
            buf[offset++] = '0' + (x / 1000000000) % 10;
        }
        if (x >= 100000000) {
            buf[offset++] = '0' + (x / 100000000) % 10;
        }
        if (x >= 10000000) {
            buf[offset++] = '0' + (x / 10000000) % 10;
        }
        buf[offset++] = '0' + (x / 1000000) % 10;
        buf[offset++] = '.';
        buf[offset++] = '0' + (x / 100000) % 10;
        buf[offset++] = '0' + (x / 10000) % 10;
        buf[offset++] = '0' + (x / 1000) % 10;
        buf[offset++] = '0' + (x / 100) % 10;
        buf[offset++] = '0' + (x / 10) % 10;
        buf[offset++] = '0' + x % 10;
        return offset;
    }
    ull three_pack(int a, int b, int c) {
        // assert (a >= -(1<<20) && a <= (1<<20)-1);
        // assert (b >= -(1<<20) && b <= (1<<20)-1);
        // assert (c >= -(1<<20) && c <= (1<<20)-1);
        return (ull(a & msk_pack) << 42 | ull(b & msk_pack) << 21 | (c & msk_pack));
    }
    ull three_pack(tiii t) {
        return three_pack(get<0>(t), get<1>(t), get<2>(t));
    }
    tiii three_unpack(ull x) {
        int a = int(x >> 42) & msk_pack;
        int b = int(x >> 21) & msk_pack;
        int c = x & msk_pack;
        if (a >> 20 == 1) {
            a |= ~msk_pack; // 111111111110(0)*20
        }
        if (b >> 20 == 1) {
            b |= ~msk_pack;
        }
        if (c >> 20 == 1) {
            c |= ~msk_pack;
        }
        return make_tuple(a, b, c);
    }
    void push_int24(vector<ubyte> &v, int x) {
        v.push_back((x >> 16) & (0xFF));
        v.push_back((x >> 8) & (0xFF));
        v.push_back(x & (0xFF));
    }
    void push_int(vector<ubyte> &v, int x) {
        v.push_back((x >> 24) & (0xFF));
        v.push_back((x >> 16) & (0xFF));
        v.push_back((x >> 8) & (0xFF));
        v.push_back(x & (0xFF));
    }
    void push_int64(vector<ubyte> &v, ull x) {
        v.push_back((x >> 56) & (0xFF));
        v.push_back((x >> 48) & (0xFF));
        v.push_back((x >> 40) & (0xFF));
        v.push_back((x >> 32) & (0xFF));
        v.push_back((x >> 24) & (0xFF));
        v.push_back((x >> 16) & (0xFF));
        v.push_back((x >> 8) & (0xFF));
        v.push_back(x & (0xFF));
    }
    fixed_byte_stream& operator >> (fixed_byte_stream &fbs, char &b) {
        b = fbs.vstr[fbs.ptr++];
        return fbs;
    }
    fixed_byte_stream& operator >> (fixed_byte_stream &fbs, ubyte &b) {
        b = fbs.vstr[fbs.ptr++];
        return fbs;
    }
    fixed_byte_stream& operator >> (fixed_byte_stream &fbs, int &v) {
        v = 0;
        for (int i = 0; i < 4; i++) {
            v <<= 8;
            v |= fbs.vstr[fbs.ptr++];
        }
        return fbs;
    }
    fixed_byte_stream& operator >> (fixed_byte_stream &fbs, ull &v) {
        v = 0;
        for (int i = 0; i < 8; i++) {
            v <<= 8;
            v |= fbs.vstr[fbs.ptr++];
        }
        return fbs;
    }
    struct node_v {
        int x, y, z;
        node_v(): x(0), y(0), z(0) {}
        node_v(int _x, int _y, int _z): x(_x), y(_y), z(_z) {}
        string dump_output_obj() const {
            string ret("v ");
            ret += int_to_str_div_1M(x);
            ret += " ";
            ret += int_to_str_div_1M(y);
            ret += " ";
            ret += int_to_str_div_1M(z);
            return ret;
        }
        int write_obj(char *buf) const {
            int offset = 0;
            buf[offset++] = 'v';
            buf[offset++] = ' ';
            offset += write_int_to_str_div_1M(buf+offset, x);
            buf[offset++] = ' ';
            offset += write_int_to_str_div_1M(buf+offset, y);
            buf[offset++] = ' ';
            offset += write_int_to_str_div_1M(buf+offset, z);
            return offset;
        }
        void to_stream(vector<ubyte> &st) {
            push_int(st, x);
            push_int(st, y);
            push_int(st, z);
        }
        bool operator == (const node_v &b) const {
            return (x == b.x && y == b.y && z == b.z);
        }
        bool operator < (const node_v &b) const {
            if (x < b.x) {
                return true;
            }
            if (b.x < x) {
                return false;
            }
            if (y < b.y) {
                return true;
            }
            if (b.y < y) {
                return false;
            }
            if (z < b.z) {
                return true;
            }
            if (b.z < z) {
                return false;
            }
            return false;
        }
    };
    fixed_byte_stream& operator >> (fixed_byte_stream &fbs, node_v &v) {
        int x, y, z;
        fbs >> x >> y >> z;
        v = node_v(x,y,z);
        return fbs;
    }
    struct node_vt {
        int x, y;
        node_vt(): x(0), y(0) {}
        node_vt(int _x, int _y): x(_x), y(_y) {}
        string dump_output_obj() const {
            string ret("vt ");
            ret += int_to_str_div_1M(x);
            ret += " ";
            ret += int_to_str_div_1M(y);
            return ret;
        }
        int write_obj(char *buf) const {
            int offset = 0;
            buf[offset++] = 'v';
            buf[offset++] = 't';
            buf[offset++] = ' ';
            offset += write_int_to_str_div_1M(buf+offset, x);
            buf[offset++] = ' ';
            offset += write_int_to_str_div_1M(buf+offset, y);
            return offset;
        }
        void to_stream(vector<ubyte> &st) {
            push_int(st, x);
            push_int(st, y);
        }
        bool operator == (const node_vt &b) const {
            return x == b.x && y == b.y;
        }
        bool operator<(const node_vt &b) const {
            if (x < b.x) {
                return true;
            }
            if (b.x < x) {
                return false;
            }
            if (y < b.y) {
                return true;
            }
            if (b.y < y) {
                return false;
            }
            return false;
        }
    };
    fixed_byte_stream& operator >> (fixed_byte_stream &fbs, node_vt &vt) {
        int x, y;
        fbs >> x >> y;
        vt = node_vt(x,y);
        return fbs;
    }
    struct node_vn {
        int x, y, z;
        node_vn(): x(0), y(0), z(0) {}
        node_vn(int _x, int _y, int _z): x(_x), y(_y), z(_z) {}
        string dump_output_obj() const {
            string ret("vn ");
            ret += int_to_str_div_1M(x);
            ret += " ";
            ret += int_to_str_div_1M(y);
            ret += " ";
            ret += int_to_str_div_1M(z);
            return ret;
        }
        int write_obj(char *buf) const {
            int offset = 0;
            buf[offset++] = 'v';
            buf[offset++] = 'n';
            buf[offset++] = ' ';
            offset += write_int_to_str_div_1M(buf+offset, x);
            buf[offset++] = ' ';
            offset += write_int_to_str_div_1M(buf+offset, y);
            buf[offset++] = ' ';
            offset += write_int_to_str_div_1M(buf+offset, z);
            return offset;
        }
        void to_stream(vector<ubyte> &st) {
            push_int64(st, three_pack(x, y, z));
        }
        bool operator == (const node_vn &b) const {
            return (x == b.x) && (y == b.y) && (z == b.z);
        }
        bool operator < (const node_vn &b) const {
            if (x < b.x) {
                return true;
            }
            if (b.x < x) {
                return false;
            }
            if (y < b.y) {
                return true;
            }
            if (b.y < y) {
                return false;
            }
            if (z < b.z) {
                return true;
            }
            if (b.z < z) {
                return false;
            }
            return false;
        }
    };
    fixed_byte_stream& operator >> (fixed_byte_stream &fbs, node_vn &vn) {
        ull t;
        fbs >> t;
        auto [a, b, c] = three_unpack(t);
        vn = node_vn(a, b, c);
        return fbs;
    }
    struct node_f {
        tiii f[3];
        node_f() {
            f[0] = make_tuple(0,0,0);
            f[1] = make_tuple(0,0,0);
            f[2] = make_tuple(0,0,0);
        }
        node_f(tiii _a, tiii _b, tiii _c) {
            f[0] = _a;
            f[1] = _b;
            f[2] = _c;
        }
        string to_str(const tiii &x) const {
            string ret;
            auto [a, b, c] = x; // C++17
            if (a) { // if a != 0
                ret += to_string(a);
            }
            ret += "/";
            if (b) { // if a != 0
                ret += to_string(b);
            }
            ret += "/";
            if (c) { // if a != 0
                ret += to_string(c);
            }
            return ret;
        }
        int write_str(char *buf, const tiii &x) const {
            int offset = 0;
            auto [p, q, r] = x;
            offset += write_pos_int_less_than_10M(buf+offset, p);
            buf[offset++] = '/';
            if (q) {
                offset += write_pos_int_less_than_10M(buf+offset, q);
            }
            buf[offset++] = '/';
            offset += write_pos_int_less_than_10M(buf+offset, r);
            return offset;
        }
        string dump_output_obj() const {
            string ret("f ");
            ret += to_str(f[0]);
            ret += " ";
            ret += to_str(f[1]);
            ret += " ";
            ret += to_str(f[2]);
            return ret;
        }
        int write_obj(char *buf) const {
            int offset = 0;
            buf[offset++] = 'f';
            buf[offset++] = ' ';
            offset += write_str(buf+offset, f[0]);
            buf[offset++] = ' ';
            offset += write_str(buf+offset, f[1]);
            buf[offset++] = ' ';
            offset += write_str(buf+offset, f[2]);
            return offset;
        }
        void to_stream(vector<ubyte> &st) {
            push_int64(st, three_pack(f[0]));
            push_int64(st, three_pack(f[1]));
            push_int64(st, three_pack(f[2]));
        }
    };
    fixed_byte_stream& operator >> (fixed_byte_stream &fbs, node_f &f) {
        ull t1, t2, t3;
        fbs >> t1 >> t2 >> t3;
        f = node_f(three_unpack(t1), three_unpack(t2), three_unpack(t3));
        return fbs;
    }
    struct delta_f {
        char dv[3] = {0,0,0}, dvt[3] = {0,0,0}, dvn[3] = {0,0,0};
        int  pv[3] = {0,0,0}, pvt[3] = {0,0,0}, pvn[3] = {0,0,0};
        delta_f() {}
        delta_f(node_f pivot) {
            for (int i = 0; i < 3; i++) {
                dv[i] = dvt[i] = dvn[i] = -128;
                pv[i] = get<0>(pivot.f[i]);
                pvt[i] = get<1>(pivot.f[i]);
                pvn[i] = get<2>(pivot.f[i]);
            }
        }
        delta_f(node_f f1, node_f f2) {
            for (int i = 0; i < 3; i++) {
                int delv = get<0>(f2.f[i]) - get<0>(f1.f[i]);
                int delvt = get<1>(f2.f[i]) - get<1>(f1.f[i]);
                int delvn = get<2>(f2.f[i]) - get<2>(f1.f[i]);
                if (delv >= -127 && delv <= 127) {
                    dv[i] = delv;
                } else {
                    dv[i] = -128;
                    pv[i] = get<0>(f2.f[i]);
                }
                if (delvt >= -127 && delvt <= 127) {
                    dvt[i] = delvt;
                } else {
                    dvt[i] = -128;
                    pvt[i] = get<1>(f2.f[i]);
                }
                if (delvn >= -127 && delvn <= 127) {
                    dvn[i] = delvn;
                } else {
                    dvn[i] = -128;
                    pvn[i] = get<2>(f2.f[i]);
                }
            }
        }
        void to_stream(vector<ubyte> &st) {
            for (int i = 0; i < 3; i++) {
                st.push_back(dv[i]);
                if (dv[i] == -128) {
                    push_int24(st, pv[i]);
                }
                st.push_back(dvt[i]);
                if (dvt[i] == -128) {
                    push_int24(st, pvt[i]);
                }
                st.push_back(dvn[i]);
                if (dvn[i] == -128) {
                    push_int24(st, pvn[i]);
                }
            }
        }
    };
    node_f operator + (const node_f &f1, const delta_f &df) {
        node_f res;
        for (int i = 0; i < 3; i++) {
            int v, vt, vn;
            v = (df.dv[i] != -128 ? get<0>(f1.f[i]) +df.dv[i] : df.pv[i]);
            vt = (df.dvt[i] != -128 ? get<1>(f1.f[i]) +df.dvt[i] : df.pvt[i]);
            vn = (df.dvn[i] != -128 ? get<2>(f1.f[i]) +df.dvn[i] : df.pvn[i]);
            res.f[i] = make_tuple(v, vt, vn);
        }
        return res;
    }
    node_f& operator += (node_f &f1, const delta_f &df) {
        for (int i = 0; i < 3; i++) {
            int v, vt, vn;
            v = (df.dv[i] != -128 ? get<0>(f1.f[i]) + df.dv[i] : df.pv[i]);
            vt = (df.dvt[i] != -128 ? get<1>(f1.f[i]) + df.dvt[i] : df.pvt[i]);
            vn = (df.dvn[i] != -128 ? get<2>(f1.f[i]) + df.dvn[i] : df.pvn[i]);
            f1.f[i] = make_tuple(v, vt, vn);
        }
        return f1;
    }
    
    fixed_byte_stream& operator >> (fixed_byte_stream &fbs, delta_f &df) {
        for (int i = 0; i < 3; i++) {
            char ch;
            fbs >> ch;
            df.dv[i] = ch;
            if (ch == -128) {
                int x = fbs.get_int24();
                df.pv[i] = x;
            }
            fbs >> ch;
            df.dvt[i] = ch;
            if (ch == -128) {
                int x = fbs.get_int24();
                df.pvt[i] = x;
            }
            fbs >> ch;
            df.dvn[i] = ch;
            if (ch == -128) {
                int x = fbs.get_int24();
                df.pvn[i] = x;
            }
        }
        return fbs;
    }
}