do {
    if(JS_TypeOfValue(cx, ${in_value}) == JSTYPE_FUNCTION)
    {
        JS::RootedObject jstarget(cx);
        if (args.thisv().isObject())
        {
            jstarget = args.thisv().toObjectOrNull();
        }
        std::shared_ptr<JSFunctionWrapper> func(new JSFunctionWrapper(cx, jstarget, ${in_value}, args.thisv()));
        auto lambda = [=](${lambda_parameters}) -> ${ret_type.name} {
            JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
            #set arg_count = len($param_types)
            #if $arg_count > 0
            JS::AutoValueVector valArr(cx);
            JS::RootedValue largv(cx);
            #end if
            #set $count = 0
            #while $count < $arg_count
                #set $arg = $param_types[$count]
            ${arg.from_native({"generator": $generator,
                                 "in_value": "larg" + str(count),
                                 "out_value": "largv",
                                 "class_name": $class_name,
                                 "level": 2,
                                 "ntype": str($arg)})};
            valArr.append(largv);
                #set $count = $count + 1
            #end while
            JS::RootedValue rval(cx);
            #if $arg_count > 0
            JS::HandleValueArray largsv(valArr);
            bool succeed = func->invoke(largsv, &rval);
            #else
            bool succeed = func->invoke(JS::HandleValueArray::empty(), &rval);
            #end if
            if (!succeed && JS_IsExceptionPending(cx)) {
                handlePendingException(cx);
            }
            #if $ret_type.name != "void"
            bool ok = true;
            ${ret_type.get_whole_name($generator)} ret;
            ${ret_type.to_native({"generator": $generator,
                                 "in_value": "rval",
                                 "out_value": "ret",
                                 "ntype": str($ret_type),
                                 "level": 2})};
            JSB_PRECONDITION2(ok, cx, false, "lambda function : Error processing return value with type ${ret_type.name}");
            return ret;
            #end if
        };
        ${out_value} = lambda;
    }
    else
    {
        ${out_value} = nullptr;
    }
} while(0)
