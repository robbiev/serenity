/*
 * Copyright (c) 2021, Idan Horowitz <idan.horowitz@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibJS/Runtime/NativeFunction.h>

namespace JS {

class WeakRefConstructor final : public NativeFunction {
    JS_OBJECT(WeakRefConstructor, NativeFunction);

public:
    explicit WeakRefConstructor(Realm&);
    virtual void initialize(Realm&) override;
    virtual ~WeakRefConstructor() override = default;

    virtual ThrowCompletionOr<Value> call() override;
    virtual ThrowCompletionOr<Object*> construct(FunctionObject&) override;

private:
    virtual bool has_constructor() const override { return true; }
};

}
