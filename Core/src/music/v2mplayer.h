/*************************************************************************************/
/*************************************************************************************/
/**                                                                                 **/
/**  V2 module player (.v2m)                                                        **/
/**  (c) Tammo 'kb' Hinrichs 2000-2008                                              **/
/**  This file is under the Artistic License 2.0, see LICENSE.txt for details       **/
/**                                                                                 **/
/*************************************************************************************/
/*************************************************************************************/

#ifndef V2MPLAYER_H_
#define V2MPLAYER_H_

#define V2MPLAYER_SYNC_FUNCTIONS
#include "libv2.h"

//#include "types.hpp"
typedef unsigned char sU8; // for packed arrays
typedef unsigned short sU16; // for packed arrays
typedef unsigned int sU32; // for packed arrays and bitfields
typedef signed char sS8; // for packed arrays
typedef short sS16; // for packed arrays
typedef int sS32; // for packed arrays
typedef float sF32; // basic floatingpoint
typedef double sF64; // use as needed
typedef int sInt; // use this most!

typedef char sChar8; // only use where the API requires ascii
typedef int sBool; // use for boolean function results

/*************************************************************************************/
/**                                                                                 **/
/**  V2M player class                                                               **/
/**                                                                                 **/
/*************************************************************************************/

class V2MPlayer {
public:
    void V2MPlayer::Tick();
    // init
    // call this instead of a constructor
    void Init(sU32 a_tickspersec = 1000)
    {
        m_tpc = a_tickspersec;
        m_base.valid = 0;
    }

    // opens a v2m file for playing
    //
    // a_v2mptr			: ptr to v2m data
    //								NOTE: the memory block has to remain valid
    //								as long as the player is opened!
    // a_samplerate : samplerate at which the synth is operating
    //                if this is zero, output will be disabled and
    //                only the timing query functions will work

    // returns  : flag if succeeded
    //
    sBool Open(const void* a_v2mptr, sU32 a_samplerate = 44100);

    // closes player
    //
    void Close();

    // starts playing
    //
    // a_time   : time offset from song start in msecs
    //
    void Play(sU32 a_time = 0);

    // stops playing
    //
    // a_fadetime : optional fade out time in msecs
    //
    void Stop(sU32 a_fadetime = 0);

    // render call (to be used from sound thread et al)
    // renders samples (or silence if not playing) into buffer
    //
    // a_buffer : ptr to stereo float sample buffer (0dB=1.0f)
    // a_len    : number of samples to render
    //
    // returns  : flag if playing
    //
    void Render(sF32* a_buffer, sU32 a_len, sBool a_add = 0);

    // render proxy for C-style callbacks
    //
    // a_this   : void ptr to instance
    // rest as in Render()
    //
    static void __stdcall RenderProxy(void* a_this, sF32* a_buffer, unsigned long a_len)
    {
        reinterpret_cast<V2MPlayer*>(a_this)->Render(a_buffer, a_len);
    }

    // returns if song is currently playing
    sBool IsPlaying();

#ifdef V2MPLAYER_SYNC_FUNCTIONS

    // Retrieves an array of timer<->song position
    //
    // a_dest: pointer to a variable which will receive the address of an array of long
    //         values structured as following:
    //         first  long: time in ms
    //         second long: song position (see above for a description)
    //											format: 0xBBBBTTNN, where
    //															BBBB is the bar number (starting at 0)
    //															TT   is the number of the 32th tick within the current bar
    //															NN   is the total number of 32th ticks a the current bar has
    //													         (32, normally, may change with different time signatures than 4/4)
    //         ... and so on for every found position
    //
    // NOTE: it is your responsibility to free the array again.
    //
    // returns: number of found song positions
    //
    sU32 CalcPositions(sS32** a_dest);

#endif

    // ------------------------------------------------------------------------------------------------------
    //  no need to look beyond this point.
    // ------------------------------------------------------------------------------------------------------

private:
    // struct defs

    // General info from V2M file
    struct V2MBase {
        sBool valid;
        const sU8* patchmap;
        const sU8* globals;
        sU32 timediv;
        sU32 timediv2;
        sU32 maxtime;
        const sU8* gptr;
        sU32 gdnum;
        struct Channel {
            sU32 notenum;
            const sU8* noteptr;
            sU32 pcnum;
            const sU8* pcptr;
            sU32 pbnum;
            const sU8* pbptr;
            struct CC {
                sU32 ccnum;
                const sU8* ccptr;
            } ctl[7];
        } chan[16];
        const char* speechdata;
        const char* speechptrs[256];
    };

    // player state
    struct PlayerState {
        enum { OFF,
            STOPPED,
            PLAYING,
        } state;
        sU32 time;
        sU32 nexttime;
        const sU8* gptr;
        sU32 gnt;
        sU32 gnr;
        sU32 usecs;
        sU32 num;
        sU32 den;
        sU32 tpq;
        sU32 bar;
        sU32 beat;
        sU32 tick;
        struct Channel {
            const sU8* noteptr;
            sU32 notenr;
            sU32 notent;
            sU8 lastnte;
            sU8 lastvel;
            const sU8* pcptr;
            sU32 pcnr;
            sU32 pcnt;
            sU8 lastpc;
            const sU8* pbptr;
            sU32 pbnr;
            sU32 pbnt;
            sU8 lastpb0;
            sU8 lastpb1;
            struct CC {
                const sU8* ccptr;
                sU32 ccnt;
                sU32 ccnr;
                sU8 lastcc;
            } ctl[7];
        } chan[16];
        sU32 cursmpl;
        sU32 smpldelta;
        sU32 smplrem;
        sU32 tdif;
    };

    // \o/
    sU8 m_synth[3 * 1024 * 1024]; // TODO: keep me uptodate or use "new"

    // member variables
    sU32 m_tpc;
    V2MBase m_base;
    PlayerState m_state;
    sU32 m_samplerate;
    sS32 m_timeoffset;
    sU8 m_midibuf[4096];
    sF32 m_fadeval;
    sF32 m_fadedelta;

    // internal methods

    sBool InitBase(const void* a_v2m); // inits base struct from v2m
    void Reset(); // resets player, inits synth
    //void Tick(); // one midi player tick
};

#endif

#define sTRUE (!0)
#define sFALSE 0

#define GETDELTA(p, w) ((p)[0] + ((p)[w] << 8) + ((p)[2 * w] << 16))
#define UPDATENT(n, v, p, w)                     \
    if ((n) < (w)) {                             \
        (v) = m_state.time + GETDELTA((p), (w)); \
        if ((v) < m_state.nexttime)              \
            m_state.nexttime = (v);              \
    }
#define UPDATENT2(n, v, p, w)                    \
    if ((n) < (w) && GETDELTA((p), (w))) {       \
        (v) = m_state.time + GETDELTA((p), (w)); \
    }
#define UPDATENT3(n, v, p, w)                \
    if ((n) < (w) && (v) < m_state.nexttime) \
        m_state.nexttime = (v);
#define PUTSTAT(s)                   \
    {                                \
        sU8 bla = (s);               \
        if (laststat != bla) {       \
            laststat = bla;          \
            *mptr++ = (sU8)laststat; \
        }                            \
    };

namespace {
void UpdateSampleDelta(sU32 nexttime, sU32 time, sU32 usecs, sU32 td2, sU32* smplrem, sU32* smpldelta)
//////////////////////////////////////////////////////////////////////////////////////////////////////
{
    // performs 64bit (nexttime-time)*usecs/td2 and a 32.32bit addition to smpldelta:smplrem
    __asm {
			mov eax, [nexttime]
			sub eax, [time]
			mov ebx, [usecs]
			mul ebx
			mov ebx, [td2]
			div ebx
			mov ecx, [smplrem]
			add[ecx], edx
			adc eax, 0
			mov ecx, [smpldelta]
			mov[ecx], eax
    }
}
}

sBool V2MPlayer::InitBase(const void* a_v2m)
///////////////////////////////////////
{
    const sU8* d = (const sU8*)a_v2m;
    m_base.timediv = (*((sU32*)(d)));
    m_base.timediv2 = 10000 * m_base.timediv;
    m_base.maxtime = *((sU32*)(d + 4));
    m_base.gdnum = *((sU32*)(d + 8));
    d += 12;
    m_base.gptr = d;
    d += 10 * m_base.gdnum;
    for (sInt ch = 0; ch < 16; ch++) {
        V2MBase::Channel& c = m_base.chan[ch];
        c.notenum = *((sU32*)d);
        d += 4;
        if (c.notenum) {
            c.noteptr = d;
            d += 5 * c.notenum;
            c.pcnum = *((sU32*)d);
            d += 4;
            c.pcptr = d;
            d += 4 * c.pcnum;
            c.pbnum = *((sU32*)d);
            d += 4;
            c.pbptr = d;
            d += 5 * c.pbnum;
            for (sInt cn = 0; cn < 7; cn++) {
                V2MBase::Channel::CC& cc = c.ctl[cn];
                cc.ccnum = *((sU32*)d);
                d += 4;
                cc.ccptr = d;
                d += 4 * cc.ccnum;
            }
        }
    }
    sInt size = *((sU32*)d);
    if (size > 16384 || size < 0)
        return sFALSE;
    d += 4;
    m_base.globals = d;
    d += size;
    size = *((sU32*)d);
    if (size > 1048576 || size < 0)
        return sFALSE;
    d += 4;
    m_base.patchmap = d;
    d += size;

    sU32 spsize = *((sU32*)d);
    d += 4;
    if (!spsize || spsize >= 8192) {
        for (sU32 i = 0; i < 256; i++)
            m_base.speechptrs[i] = " ";
    } else {
        m_base.speechdata = (const char*)d;
        d += spsize;
        const sU32* p32 = (const sU32*)m_base.speechdata;
        sU32 n = *(p32++);
        for (sU32 i = 0; i < n; i++) {
            m_base.speechptrs[i] = m_base.speechdata + *(p32++);
        }
    }

    return sTRUE;
}

void V2MPlayer::Reset()
////////////////////////
{
    m_state.time = 0;
    m_state.nexttime = (sU32)-1;

    m_state.gptr = m_base.gptr;
    m_state.gnr = 0;
    UPDATENT(m_state.gnr, m_state.gnt, m_state.gptr, m_base.gdnum);
    for (sInt ch = 0; ch < 16; ch++) {
        V2MBase::Channel& bc = m_base.chan[ch];
        PlayerState::Channel& sc = m_state.chan[ch];

        if (!bc.notenum)
            continue;
        sc.noteptr = bc.noteptr;
        sc.notenr = sc.lastnte = sc.lastvel = 0;
        UPDATENT(sc.notenr, sc.notent, sc.noteptr, bc.notenum);
        sc.pcptr = bc.pcptr;
        sc.pcnr = sc.lastpc = 0;
        UPDATENT(sc.pcnr, sc.pcnt, sc.pcptr, bc.pcnum);
        sc.pbptr = bc.pbptr;
        sc.pbnr = sc.lastpb0 = sc.lastpb1 = 0;
        UPDATENT(sc.pbnr, sc.pbnt, sc.pbptr, bc.pcnum);
        for (sInt cn = 0; cn < 7; cn++) {
            V2MBase::Channel::CC& bcc = bc.ctl[cn];
            PlayerState::Channel::CC& scc = sc.ctl[cn];
            scc.ccptr = bcc.ccptr;
            scc.ccnr = scc.lastcc = 0;
            UPDATENT(scc.ccnr, scc.ccnt, scc.ccptr, bcc.ccnum);
        }
    }
    m_state.usecs = 5000 * m_samplerate;
    m_state.num = 4;
    m_state.den = 4;
    m_state.tpq = 8;
    m_state.bar = 0;
    m_state.beat = 0;
    m_state.tick = 0;
    m_state.smplrem = 0;

    if (m_samplerate) {
        synthInit(m_synth, (void*)m_base.patchmap, m_samplerate);
        synthSetGlobals(m_synth, (void*)m_base.globals);
        synthSetLyrics(m_synth, m_base.speechptrs);
    }
}

void V2MPlayer::Tick()
///////////////////////
{
    if (m_state.state != PlayerState::PLAYING)
        return;

    m_state.tick += m_state.nexttime - m_state.time;
    while (m_state.tick >= m_base.timediv) {
        m_state.tick -= m_base.timediv;
        m_state.beat++;
    }
    sU32 qpb = (m_state.num * 4 / m_state.den);
    while (m_state.beat >= qpb) {
        m_state.beat -= qpb;
        m_state.bar++;
    }

    m_state.time = m_state.nexttime;
    m_state.nexttime = (sU32)-1;
    sU8* mptr = m_midibuf;
    sU32 laststat = -1;

    if (m_state.gnr < m_base.gdnum && m_state.time == m_state.gnt) // neues global-event?
    {
        m_state.usecs = (*(sU32*)(m_state.gptr + 3 * m_base.gdnum + 4 * m_state.gnr)) * (m_samplerate / 100);
        m_state.num = m_state.gptr[7 * m_base.gdnum + m_state.gnr];
        m_state.den = m_state.gptr[8 * m_base.gdnum + m_state.gnr];
        m_state.tpq = m_state.gptr[9 * m_base.gdnum + m_state.gnr];
        m_state.gnr++;
        UPDATENT2(m_state.gnr, m_state.gnt, m_state.gptr + m_state.gnr, m_base.gdnum);
    }
    UPDATENT3(m_state.gnr, m_state.gnt, m_state.gptr + m_state.gnr, m_base.gdnum);

    for (sInt ch = 0; ch < 16; ch++) {
        V2MBase::Channel& bc = m_base.chan[ch];
        PlayerState::Channel& sc = m_state.chan[ch];
        if (!bc.notenum)
            continue;
        // 1. process pgm change events
        if (sc.pcnr < bc.pcnum && m_state.time == sc.pcnt) {
            PUTSTAT(0xc0 | ch)
            *mptr++ = (sc.lastpc += sc.pcptr[3 * bc.pcnum]);
            sc.pcnr++;
            sc.pcptr++;
            UPDATENT2(sc.pcnr, sc.pcnt, sc.pcptr, bc.pcnum);
        }
        UPDATENT3(sc.pcnr, sc.pcnt, sc.pcptr, bc.pcnum);

        // 2. process control changes
        for (sInt cn = 0; cn < 7; cn++) {
            V2MBase::Channel::CC& bcc = bc.ctl[cn];
            PlayerState::Channel::CC& scc = sc.ctl[cn];
            if (scc.ccnr < bcc.ccnum && m_state.time == scc.ccnt) {
                PUTSTAT(0xb0 | ch)
                *mptr++ = cn + 1;
                *mptr++ = (scc.lastcc += scc.ccptr[3 * bcc.ccnum]);
                scc.ccnr++;
                scc.ccptr++;
                UPDATENT2(scc.ccnr, scc.ccnt, scc.ccptr, bcc.ccnum);
            }
            UPDATENT3(scc.ccnr, scc.ccnt, scc.ccptr, bcc.ccnum);
        }

        // 3. process pitch bends
        if (sc.pbnr < bc.pbnum && m_state.time == sc.pbnt) {
            PUTSTAT(0xe0 | ch)
            *mptr++ = (sc.lastpb0 += sc.pbptr[3 * bc.pcnum]);
            *mptr++ = (sc.lastpb1 += sc.pbptr[4 * bc.pcnum]);
            sc.pbnr++;
            sc.pbptr++;
            UPDATENT2(sc.pbnr, sc.pbnt, sc.pbptr, bc.pbnum);
        }
        UPDATENT3(sc.pbnr, sc.pbnt, sc.pbptr, bc.pbnum);

        // 4. process notes
        while (sc.notenr < bc.notenum && m_state.time == sc.notent) {
            PUTSTAT(0x90 | ch)
            *mptr++ = (sc.lastnte += sc.noteptr[3 * bc.notenum]);
            *mptr++ = (sc.lastvel += sc.noteptr[4 * bc.notenum]);
            sc.notenr++;
            sc.noteptr++;
            UPDATENT2(sc.notenr, sc.notent, sc.noteptr, bc.notenum);
        }
        UPDATENT3(sc.notenr, sc.notent, sc.noteptr, bc.notenum);
    }

    *mptr++ = 0xfd;

    synthProcessMIDI(m_synth, m_midibuf);

    if (m_state.nexttime == (sU32)-1)
        m_state.state = PlayerState::STOPPED;
}

sBool V2MPlayer::Open(const void* a_v2mptr, sU32 a_samplerate)
///////////////////////////////////////////////////////////////
{
    if (m_base.valid)
        Close();

    m_samplerate = a_samplerate;

    if (!InitBase(a_v2mptr))
        return sFALSE;

    Reset();

    return m_base.valid = sTRUE;
}

void V2MPlayer::Close()
////////////////////////
{
    if (!m_base.valid)
        return;
    if (m_state.state != PlayerState::OFF)
        Stop();

    m_base.valid = 0;
}

void V2MPlayer::Play(sU32 a_time)
//////////////////////////////////
{
    if (!m_base.valid || !m_samplerate)
        return;

    Stop();
    Reset();

    m_base.valid = sFALSE;
    sU32 destsmpl, cursmpl = 0;
    __asm
        {
		mov  ecx, this
		mov	 eax, [a_time]
		mov  ebx, [ecx + m_samplerate]
		imul ebx
		mov  ebx, [ecx + m_tpc]
		idiv ebx
		mov[destsmpl], eax
        }

    m_state.state = PlayerState::PLAYING;
    m_state.smpldelta = 0;
    m_state.smplrem = 0;
    while ((cursmpl + m_state.smpldelta) < destsmpl && m_state.state == PlayerState::PLAYING) {
        cursmpl += m_state.smpldelta;
        Tick();
        if (m_state.state == PlayerState::PLAYING) {
            UpdateSampleDelta(m_state.nexttime, m_state.time, m_state.usecs, m_base.timediv2, &m_state.smplrem, &m_state.smpldelta);
        } else
            m_state.smpldelta = -1;
    }
    m_state.smpldelta -= (destsmpl - cursmpl);
    m_timeoffset = cursmpl - m_state.cursmpl;
    m_fadeval = 1.0f;
    m_fadedelta = 0.0f;
    m_base.valid = sTRUE;
}

void V2MPlayer::Stop(sU32 a_fadetime)
//////////////////////////////////////
{
    if (!m_base.valid)
        return;

    if (a_fadetime) {
        sU32 ftsmpls;
        __asm
            {
			mov  ecx, this
			mov	 eax, [a_fadetime]
			mov  ebx, [ecx + m_samplerate]
			imul ebx
			mov  ebx, [ecx + m_tpc]
			idiv ebx
			mov[ftsmpls], eax
            }
        m_fadedelta = m_fadeval / ftsmpls;
    } else
        m_state.state = PlayerState::OFF;
}

void V2MPlayer::Render(sF32* a_buffer, sU32 a_len, sBool a_add)
/////////////////////////////////////////////////////////////////
{
    if (!a_buffer)
        return;

    if (m_base.valid && m_state.state == PlayerState::PLAYING) {
        sU32 todo = a_len;
        while (todo) {
            sInt torender = (todo > m_state.smpldelta) ? m_state.smpldelta : todo;
            if (torender) {
                synthRender(m_synth, a_buffer, torender, 0, a_add);
                a_buffer += 2 * torender;
                todo -= torender;
                m_state.smpldelta -= torender;
                m_state.cursmpl += torender;
            }
            if (!m_state.smpldelta) {
                Tick();
                if (m_state.state == PlayerState::PLAYING)
                    UpdateSampleDelta(m_state.nexttime, m_state.time, m_state.usecs, m_base.timediv2, &m_state.smplrem, &m_state.smpldelta);
                else
                    m_state.smpldelta = -1;
            }
        }
    } else if (m_state.state == PlayerState::OFF || !m_base.valid) {
        if (!a_add) {
            __asm {
				mov edi, [a_buffer]
				mov ecx, [a_len]
				shl ecx, 1
				xor eax, eax
				rep stosd
            }
        }
    } else {
        synthRender(m_synth, a_buffer, a_len, 0, a_add);
        m_state.cursmpl += a_len;
    }

    if (m_fadedelta) {
        for (sU32 i = 0; i < a_len; i++) {
            a_buffer[2 * i] *= m_fadeval;
            a_buffer[2 * i + 1] *= m_fadeval;
            m_fadeval -= m_fadedelta;
            if (m_fadeval < 0)
                m_fadeval = 0;
        }
        if (!m_fadeval)
            Stop();
    }
}

sBool V2MPlayer::IsPlaying()
{
    return m_base.valid && m_state.state == PlayerState::PLAYING;
}

#ifdef V2MPLAYER_SYNC_FUNCTIONS

sU32 V2MPlayer::CalcPositions(sS32** a_dest)
/////////////////////////////////////////////
{
    if (!a_dest)
        return 0;
    if (!m_base.valid) {
        *a_dest = 0;
        return 0;
    }

    // step 1: ende finden
    sS32*& dp = *a_dest;
    sU32 gnr = 0;
    const sU8* gp = m_base.gptr;
    sU32 curbar = 0;
    sU32 cur32th = 0;
    sU32 lastevtime = 0;
    sU32 pb32 = 32;
    sU32 usecs = 500000;

    sU32 posnum = 0;
    sU32 ttime, td, this32;
    sF64 curtimer = 0;

    while (gnr < m_base.gdnum) {
        ttime = lastevtime + (gp[2 * m_base.gdnum] << 16) + (gp[m_base.gdnum] << 8) + gp[0];
        td = ttime - lastevtime;
        this32 = (td * 8 / m_base.timediv);
        posnum += this32;
        lastevtime = ttime;
        pb32 = gp[7 * m_base.gdnum] * 32 / gp[8 * m_base.gdnum];
        gnr++;
        gp++;
    }
    td = m_base.maxtime - lastevtime;
    this32 = (td * 8 / m_base.timediv);
    posnum += this32 + 1;
    dp = new sS32[2 * posnum];
    gnr = 0;
    gp = m_base.gptr;
    lastevtime = 0;
    pb32 = 32;
    sU32 pn;
    for (pn = 0; pn < posnum; pn++) {
        sU32 curtime = pn * m_base.timediv / 8;
        if (gnr < m_base.gdnum) {
            ttime = lastevtime + (gp[2 * m_base.gdnum + gnr] << 16) + (gp[m_base.gdnum + gnr] << 8) + gp[gnr];
            if (curtime >= ttime) {
                pb32 = gp[7 * m_base.gdnum + gnr] * 32 / gp[8 * m_base.gdnum + gnr];
                usecs = *(sU32*)(gp + 3 * m_base.gdnum + 4 * gnr);
                gnr++;
                lastevtime = ttime;
            }
        }
        dp[2 * pn] = (sU32)curtimer;
        dp[2 * pn + 1] = (curbar << 16) | (cur32th << 8) | (pb32);

        cur32th++;
        if (cur32th == pb32) {
            cur32th = 0;
            curbar++;
        }
        curtimer += m_tpc * usecs / 8000000.0;
    }
    return pn;
}

#endif