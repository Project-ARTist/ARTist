/**
 * The ARTist Project (https://artist.cispa.saarland)
 *
 * Copyright (C) 2017 CISPA (https://cispa.saarland), Saarland University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author "Oliver Schranz <oliver.schranz@cispa.saarland>"
 * @author "Sebastian Weisgerber <weisgerber@cispa.saarland>"
 *
 */

#include "artist.h"
#include "artist_log.h"
#include "artist_method_visitor.h"

namespace art {

HArtistMethodVisitor::HArtistMethodVisitor(HGraph* graph)
    : HGraphVisitor(graph) {
  DCHECK(graph != nullptr);
}

void HArtistMethodVisitor::VisitReturn(HReturn* instruction) {
  DCHECK(instruction != nullptr);
  ArtistLog::LogInstructionName(instruction);
}

void HArtistMethodVisitor::VisitReturnVoid(HReturnVoid* instruction) {
  DCHECK(instruction != nullptr);
  ArtistLog::LogInstructionName(instruction);
}

void HArtistMethodVisitor::VisitInvokeInterface(HInvokeInterface* instruction) {
  DCHECK(instruction != nullptr);
  ArtistLog::LogInstructionName(instruction);
}

void HArtistMethodVisitor::VisitInvokeStaticOrDirect(HInvokeStaticOrDirect* instruction) {
  DCHECK(instruction != nullptr);
  ArtistLog::LogInstructionName(instruction);
}

void HArtistMethodVisitor::VisitInvokeVirtual(HInvokeVirtual* instruction) {
  DCHECK(instruction != nullptr);
  ArtistLog::LogInstructionName(instruction);
}


void HArtistMethodVisitor::VisitStaticFieldSet(HStaticFieldSet* instruction) {
  DCHECK(instruction != nullptr);
  ArtistLog::LogInstructionName(instruction);
}

void HArtistMethodVisitor::VisitInstanceFieldSet(HInstanceFieldSet* instruction) {
  DCHECK(instruction != nullptr);
  ArtistLog::LogInstructionName(instruction);
}

void HArtistMethodVisitor::VisitParameterValue(HParameterValue* instruction) {
  DCHECK(instruction != nullptr);
  ArtistLog::LogInstructionName(instruction);
}

void HArtistMethodVisitor::VisitInstruction(HInstruction* instruction) {
  DCHECK(instruction != nullptr);
  ArtistLog::LogInstructionName(instruction);
}

}  // namespace art

