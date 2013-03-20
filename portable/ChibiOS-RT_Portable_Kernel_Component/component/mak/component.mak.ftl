[#ftl]
[@pp.dropOutputFile /]
[@pp.changeOutputFile name="component.mak" /]
# List of library files exported by the component.
LIB_C_SRC       += ${global.component_path}/lib/src/chsys.c \
                   ${global.component_path}/lib/src/chdebug.c \
                   ${global.component_path}/lib/src/chlists.c \
                   ${global.component_path}/lib/src/chvt.c \
                   ${global.component_path}/lib/src/chschd.c \
                   ${global.component_path}/lib/src/chthreads.c \
[#if conf.instance.subsystem_options.use_registry[0]?lower_case == "true"]
                   ${global.component_path}/lib/src/chdynamic.c \
[/#if]
[#if conf.instance.subsystem_options.use_dynamic_extensions[0]?lower_case == "true"]
                   ${global.component_path}/lib/src/chregistry.c \
[/#if]
[#if conf.instance.subsystem_options.use_semaphores[0]?lower_case == "true"]
                   ${global.component_path}/lib/src/chsem.c \
[/#if]
[#if conf.instance.subsystem_options.use_mutexes[0]?lower_case == "true"]
                   ${global.component_path}/lib/src/chmtx.c \
[/#if]
[#if conf.instance.subsystem_options.use_condition_variables[0]?lower_case == "true"]
                   ${global.component_path}/lib/src/chcond.c \
[/#if]
[#if conf.instance.subsystem_options.use_events[0]?lower_case == "true"]
                   ${global.component_path}/lib/src/chevents.c \
[/#if]
[#if conf.instance.subsystem_options.use_messages[0]?lower_case == "true"]
                   ${global.component_path}/lib/src/chmsg.c \
[/#if]
[#if conf.instance.subsystem_options.use_mailboxes[0]?lower_case == "true"]
                   ${global.component_path}/lib/src/chmboxes.c \
[/#if]
[#if conf.instance.subsystem_options.use_queues[0]?lower_case == "true"]
                   ${global.component_path}/lib/src/chqueues.c \
[/#if]
[#if conf.instance.subsystem_options.use_memory_core_allocator[0]?lower_case == "true"]
                   ${global.component_path}/lib/src/chmemcore.c \
[/#if]
[#if conf.instance.subsystem_options.use_heap_allocator[0]?lower_case == "true"]
                   ${global.component_path}/lib/src/chheap.c \
[/#if]
[#if conf.instance.subsystem_options.use_memory_pools_allocator[0]?lower_case == "true"]
                   ${global.component_path}/lib/src/chmempools.c \
[/#if]

LIB_CPP_SRC     +=

LIB_ASM_SRC     +=

LIB_INCLUDES    += ${global.component_path}/lib/include

APP_C_SRC       +=

APP_CPP_SRC     +=

APP_ASM_SRC     +=

APP_INCLUDES    += ${global.local_component_path}/cfg
