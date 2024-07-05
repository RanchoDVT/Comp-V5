# VEXcode mkrules.mk 2019_03_26_01

# Compile C files
$(BUILD)/%.o: %.c $(SRC_H)
	$(Q)$(MKDIR)
	$(ECHO) "CC  $<"
	$(Q)$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	
# Compile C++ files
$(BUILD)/%.o: %.cpp $(SRC_H) $(SRC_A)
	$(Q)$(MKDIR)
	$(ECHO) "CXX $<"
	$(Q)$(CXX) $(CXX_FLAGS) $(INC) -c -o $@ $<
	
# Create executable 
$(BUILD)/$(PROJECT).elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LINK) $(LNK_FLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

# Create binary 
$(BUILD)/$(PROJECT).bin: $(BUILD)/$(PROJECT).elf
	$(Q)$(OBJCOPY) -O binary $(BUILD)/$(PROJECT).elf $(BUILD)/$(PROJECT).bin

# Create archive
$(BUILD)/$(PROJECTLIB).a: $(OBJ)
	$(Q)$(ARCH) $(ARCH_FLAGS) $@ $^

# Clean project
clean:
	$(info clean project)
	$(Q)$(CLEAN)
