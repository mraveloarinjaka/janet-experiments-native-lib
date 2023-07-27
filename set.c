#include <janet.h>

static int set_gc(void *data, size_t len) {
   (void)len;
   janet_table_deinit((JanetTable *)data);
   return 0;
}

static int set_gcmark(void *data, size_t len) {
   (void)len;
   janet_mark(janet_wrap_table((JanetTable *)data));
   return 0;
}

static void set_tostring(void *data, JanetBuffer *buffer) {
   JanetTable *set = (JanetTable *)data;
   janet_buffer_push_cstring(buffer, "{");
   int first = 1;
   for (int32_t i = 0; i < set->capacity; i++) {
      JanetKV *entry = &set->data[i];
      if (janet_checktype(entry->key, JANET_NIL)) {
         continue;
      }
      if (first) {
         first = 0;
      } else {
         janet_buffer_push_cstring(buffer, " ");
      }
      janet_pretty(buffer, 0, 0, entry->key);
   }
   janet_buffer_push_cstring(buffer, "}");
}

static const JanetAbstractType set_type = {
    .name = "set",
    .gc = set_gc,
    .gcmark = set_gcmark,
    .get = NULL,
    .put = NULL,
    .marshal = NULL,
    .unmarshal = NULL,
    .tostring = set_tostring,
    .compare = NULL,
    .hash = NULL,
    .next = NULL,
    .call = NULL,
    .length = NULL,
    .bytes = NULL,
};

static JanetTable *new_abstract_set(int32_t argc) {
   JanetTable *set =
       (JanetTable *)janet_abstract(&set_type, sizeof(JanetTable));
   set->gc = (JanetGCObject){0, NULL};
   janet_table_init_raw(set, argc);
   return set;
}

static Janet cfun_new(int32_t argc, Janet *argv) {
   JanetTable *set = new_abstract_set(argc);
   for (int32_t i = 0; i < argc; i++) {
      janet_table_put(set, argv[i], janet_wrap_true());
   }
   return janet_wrap_abstract(set);
}

static Janet cfun_hello(int32_t argc, Janet *argv) {
   janet_fixarity(argc, 0);
   printf("hello world\n");
   return janet_wrap_nil();
}

static JanetReg cfuns[] = {
    {"hello", cfun_hello, "(hello)\n\nprints hello"},
    {"new", cfun_new,
     "(set/new & xs)\n\n"
     "Returns a set containing only this function's arguments."},
    {NULL, NULL, NULL}};

JANET_MODULE_ENTRY(JanetTable *env) {
   janet_cfuns(env, "set", cfuns);
   janet_register_abstract_type(&set_type);
}
