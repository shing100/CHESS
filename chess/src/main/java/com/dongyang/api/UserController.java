package com.dongyang.api;

import com.dongyang.domain.User;
import com.dongyang.repository.UserRepository;
import com.google.gson.JsonObject;
import org.apache.catalina.servlet4preview.http.HttpServletRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.bind.annotation.RestController;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by HeemangHan on 2016. 10. 25..
 */

@Controller
@RequestMapping("user")
public class UserController {

    @Autowired
    UserRepository userRepository;

    @RequestMapping(value = "/login.do", method = RequestMethod.POST)
    public String login(HttpServletRequest request) {
        String returnURL = "";
        String id = request.getParameter("id");
        String password = request.getParameter("password");

        User user = userRepository.findByIdAndPassword(id, password);

        if(user != null) {
            request.getSession().setAttribute("admin", user);
            returnURL = "redirect:/main";
        } else {
            returnURL = "redirect:/login";
        }

        return returnURL;
    }

    @RequestMapping(value = "/logout.do")
    public String logout(HttpServletRequest request) {
        request.getSession().invalidate();

        return "redirect:/login";
    }

    @ResponseBody
    @RequestMapping(value = "/change.do", method = RequestMethod.POST)
    public Map<String, Object> changePassword(HttpServletRequest request) {
        Map<String, Object> jsonObject = new HashMap<String, Object>();
        User user;
        String result = "";
        String message = "";


        String oriPwd = request.getParameter("oriPwd");
        String chgPwd = request.getParameter("chgPwd");

        user = userRepository.findByIdAndPassword("admin", oriPwd);

        if(user != null) {
            int updateResult = userRepository.updateByPassword(chgPwd);

            if(updateResult != 0) {
                result = "success";
                message = "success change password";
            } else {
                result = "failed";
                message = "SQL error";
            }
        } else {
            result = "failed";
            message = "wrong password";
        }

        jsonObject.put("result", result);
        jsonObject.put("message", message);

        return jsonObject;
    }
}
