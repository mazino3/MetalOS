#include "KSpinLock.h"
#include "x64_support.h"

KSpinLock::KSpinLock() : m_value(Unlocked)
{

}

cpu_flags_t KSpinLock::Acquire()
{
	cpu_flags_t flags = x64_disable_interrupts();

	static_assert(std::numeric_limits<size_t>::digits == 64);
	while (_InterlockedCompareExchange64((volatile long long*)&m_value, Unlocked, Locked) != Unlocked)
		x64_pause();

	return flags;
}

void KSpinLock::Release(cpu_flags_t flags)
{
	m_value = Unlocked;
	x64_restore_flags(flags);
}